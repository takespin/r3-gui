#ifndef CALCR3_H
#define CALCR3_H
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QString>
#include <QVector>

#include "gamma.h"

#include "fid.h"

#define MAXSPINS 2
#define NPROP 20500

#define TWOPI 2*PI
#define FOURPI 12.566370614
#define PLANCKHBAR 1.054E-34 // J s rad^-1

class calcr3Thread : public QThread
{
    Q_OBJECT

public:
    calcr3Thread();
    ~calcr3Thread()
    {
        mutex.lock();
          stopped=true;
          condition.wakeOne();
        mutex.unlock();
        wait();

        delete fid_2d;
    }

    void doCalc();
    void stop() { QMutexLocker locker(&mutex); stopped=true; condition.wakeAll();}


    void setMagneticField(double d) {FMagneticField=d;}
    void setGammaI(double d) {FGammaI=d;}
    void setGammaS(double d) {FGammaS=d;}
    void setDistance(double d) {FDistance=d;}
    void setSpinningSpeed(double d) {FSpinningSpeed=d;}
    void setNutationSpeed(double d) {FNutationSpeed=d;}
    void setAngleIncrement(double d) {FAngleIncrement=d;}
    void setFileName(QString qs) {FFileName=qs;}
    void setNSteps(int k) {FNSteps=k;}
    void setAl(int k) {FAl=k;}
    void setNObs(int k) {FNObs=k;}

signals:
    void sendMessage(QString qs);
    void calcComplete();

public slots:
    double magneticField() {return FMagneticField;}
    double gammaI() {return FGammaI;}
    double gammaS() {return FGammaS;}
    double distance() {return FDistance;}
    double spinningSpeed() {return FSpinningSpeed;}
    double nutationSpeed() {return FNutationSpeed;}
    double angleIncrement() {return FAngleIncrement;}
    QString fileName() {return FFileName;}
    int nSteps() {return FNSteps;}
    int al() {return FAl;}
    int nObs() {return FNObs;}

protected:
    void run();

private:
    QMutex mutex;
    QWaitCondition condition;
    bool volatile stopped;

    double FMagneticField;
    double FGammaI;
    double FGammaS;
    double FDistance;
    double FSpinningSpeed;
    double FNutationSpeed;
    double FAngleIncrement;
    QString FFileName;
    int FNSteps;
    int FAl;
    int FNObs;

    bool prepareCalc();

    enum qState {L1,L2,B1,E};
    enum qState currentState;

    gen_op H[5]; // irreducible representation
    gen_op Hamiltonian;
    gen_op U[NPROP],Udag[NPROP];
    gen_op rho,detect;

    spin_T  Hdip[MAXSPINS][MAXSPINS];
    space_T Adip[MAXSPINS][MAXSPINS], Adip_R[MAXSPINS][MAXSPINS];
    space_T Acsa[MAXSPINS], Acsa_R[MAXSPINS];
    double D[MAXSPINS][MAXSPINS];
    double iso_CSA[MAXSPINS];
    double eta_CSA[MAXSPINS];
    double delta_CSA[MAXSPINS];

    double alpha_CSA[MAXSPINS],beta_CSA[MAXSPINS],gamma_CSA[MAXSPINS];
    double alpha_D[MAXSPINS][MAXSPINS],beta_D[MAXSPINS][MAXSPINS],gamma_D[MAXSPINS][MAXSPINS];


    int nspins = 2;
    spin_sys ax;


    TFID_2D *fid_2d;

};

#endif // CALCR3_H
