#include "calcr3.h"
#include "math.h"
#include <QDebug>


calcr3Thread::calcr3Thread()
{
    fid_2d = new TFID_2D();
    setPriority(QThread::NormalPriority);

}


void calcr3Thread::doCalc()
{
  QMutexLocker locker(&mutex);
  stopped=false;
  if(!isRunning())
  {
    start(priority());
  }
  else
  {
    condition.wakeOne();
  }
}


bool calcr3Thread::prepareCalc()
{
    ax = spin_sys(nspins);

    ax.isotope(0,"13C");
    ax.isotope(1,"13C");

    // dipolar coupling constant in Hz
    double dis;
    dis=10*6.626*gammaI()*gammaS()/(distance()*distance()*distance());
    // 10 comes from 1E-7.1E-34.1E6.1E6.1E30 = 1E1

    D[0][1]=dis;
    alpha_D[0][1]=0; beta_D[0][1]=0; gamma_D[0][1]=0;

    iso_CSA[0]=isoS();
    delta_CSA[0]=0.0e3; eta_CSA[0]=0.0;
    alpha_CSA[0]=0.0; beta_CSA[0]=0.0; gamma_CSA[0]=0.0;

    iso_CSA[1]=isoI();
    delta_CSA[1]=0.0; eta_CSA[1]=0.0;
    alpha_CSA[1]=0.0; beta_CSA[1]=0.0; gamma_CSA[1]=0.0;

    //----- setup for the hamiltonian -----
    for(int i=0;i<nspins-1;++i)
    { for(int j=i+1;j<nspins;++j)
      { Hdip[i][j] = T_D(ax,i,j);
      }
    }

    matrix help(3,3,0);

    //----- setup for the space tensor -----
    for(int i=0;i<nspins-1;++i)
    { for(int j=i+1;j<nspins;++j)
      { help.put_h(-1.0,0,0);   //
        help.put_h(-1.0,1,1);   // I convinced myself! (2013.9.18 K.T)
        help.put_h( 2.0,2,2);   //
        help   = -complex(D[i][j],0) * help;
        Adip[i][j] = A2(help);
        Adip[i][j] = Adip[i][j].rotate(alpha_D[i][j],beta_D[i][j],gamma_D[i][j]);
      }
    }

    for(int i=0;i<nspins;++i)
    { help.put(-1.0/2.0*(1.0-eta_CSA[i]),0,0);  //
      help.put(-1.0/2.0*(1.0+eta_CSA[i]),1,1);  // OK! (2013.9.18 K.T)
      help.put( 1.0,2,2);                       //
      help = -complex(delta_CSA[i],0) * help;
      Acsa[i] = A2(help);
      Acsa[i] = Acsa[i].rotate(alpha_CSA[i],beta_CSA[i],gamma_CSA[i]);
    }

    fid_2d->FID.clear();
    fid_2d->setAl(al());
    fid_2d->FID.append(new TFID(al()));
    fid_2d->FID.last()->setEmpty(false);
    double dt=1.0/(1000*spinningSpeed()*nSteps());
    fid_2d->setDW(dt*nObs()*1e6);
    fid_2d->setSF1(magneticField()*gammaS());

    emit xRangeUpdateRequest(al());

    return true;
}





void calcr3Thread::run()
{
  forever
  {
    if (stopped) return;
    if (!prepareCalc()) return;

    int i,j,k,q,cobs,cal,cevolve; // counter
    int nevolve=20;

    const double thetam=54.73561032;  // magic angle

    double t; // current time and time increment in us
    double scale;  // a weight factor

    for(double alpha=0; alpha<360; alpha+=360)//angleIncrement())
    {
    for(double beta=angleIncrement(); beta<180; beta+=angleIncrement())
    {
      scale = sin(PI*beta/180.0);
    for(double gamma=0; gamma<360; gamma+=angleIncrement()/scale)
    {

        if(stopped) return;
        //rotate the space tensor
        for(int i=0;i<nspins;++i)
        {
          Acsa_R[i] = Acsa[i].rotate(alpha,beta,gamma);
          for(j=i+1;j<nspins;++j) Adip_R[i][j] = Adip[i][j].rotate(alpha,beta,gamma);
        }

        //zero all components
        for(int i=0;i<5;++i) H[i] = gen_op();

        //now we include the spin part of the Hamiltonian
        for(i=0;i<nspins;++i) H[2] += iso_CSA[i]*Iz(ax,i); // isotropic chemical shift


        for(k=-2;k<=2;++k)
        {
          for(i=0;i<nspins-1;++i)
          {
             H[k+2] += Acsa_R[i].component(2,k) * d2(k,0,thetam)*2.0/sqrt(6.0)*Iz(ax,i);  // CSA
             for(j=i+1;j<nspins;++j)
             {
               if(ax.isotope(i) != ax.isotope(j))
               { // heteronuclear dipolar interaction
                 H[k+2] += Adip_R[i][j].component(2,k) * d2(k,0,thetam) * 2.0/sqrt(6.0)*Iz(ax,i)*Iz(ax,j);
               }
               else
               { // homonuclear dipolar interaction
                 H[k+2] += Adip_R[i][j].component(2,k) * d2(k,0,thetam) * Hdip[i][j].component(2,0);
               }
             }
           }
         }

        t=0.0;
        double dt=1.0/(1000*spinningSpeed()*nSteps());
        cevolve=0;
        cobs=0;
        cal=0;
        q=0;
        complex cdat;

        double w;

        rho  = Fx(ax,"13C");
        detect = Fm(ax,"13C");


        currentState = L1;

        while (currentState!=E) {
        switch(currentState) {

          case L1: // time evolution operator for one rotor period
            if(cevolve>=NPROP)
            {
              emit sendMessage("Time resolution is too high.");
              return;
            }

            Hamiltonian=0;
            for(i=-2;i<=2;++i)
            {
              Hamiltonian += exp(complex(0,i*2.0*PI*t*1000*spinningSpeed())) * H[i+2];
            }
            Hamiltonian += 1000*nutationSpeed() * Ix(ax,1); // cw irradiation at spin I

            // time evolution operator
            U[cevolve]=prop(Hamiltonian,dt);
            Udag[cevolve]=adjoint(U[cevolve]);
            t+=dt;

            cevolve++;
            if(t*1000*spinningSpeed()>=1.0) // one rotor period?
            {
              nevolve = cevolve; //cout << "nevolve:" << nevolve;
              cevolve = 0;
              cobs = 0;
              cal = 0;
              currentState = B1;
            }
            else
            {
              currentState = L1;
            }
            break;

          case L2: // perform time evolution
            if (cevolve==nevolve) {cevolve=0;}
            rho=U[cevolve]*rho*Udag[cevolve];
            cevolve++;
            cobs++;
            if (cobs==nObs()) {cobs=0; currentState = B1; break;}
            currentState = L2;
            break;

          case B1:
//            cdat = proj(rho,detect)*scale;
            cdat = proj(rho,detect);

            // Gaussian apodization
            w=exp(-cal*cal*(fid_2d->dw()/1000000.0)*(fid_2d->dw()/1000000.0)*apodizationWidth()*apodizationWidth()*3.55971);
                                     // (2pi) ^2/(16 ln2) = 3.55971


            fid_2d->FID[0]->real->sig[cal] += w * cdat.real();
            fid_2d->FID[0]->imag->sig[cal] += w * cdat.imag();
            cal++;
            if (cal==al()) {cal=0; currentState=E;} else {currentState=L2;}
            break;

          case E:
            break;
        }  // switch
        } // while (currentState!=E)

    fid_2d->FID[0]->updateAbs();

    QString mes= //"a:"+QString::number(alpha) +
               "beta: "+QString::number(beta) +", gamma: "+QString::number(gamma);
    emit sendMessage(mes);
    emit dataUpdated();

    mutex.lock();
    fid_2d->WriteopFiles(fileName());
    mutex.unlock();

    } // gamma
    } // beta
    } // alpha


    emit sendMessage("Complete!");

    emit calcComplete();
    mutex.lock();
    condition.wait(&mutex);   // Let the thread sleep.
    mutex.unlock();
  } // forever
}
