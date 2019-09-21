#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include "math.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    calcInProgress=false;

    resize(QSize(400,600));
    createWidgets();
    createPanel();

    calcThread = new calcr3Thread;

    connect(calcThread,SIGNAL(calcComplete()),this,SLOT(onStartStopButtonClicked()));
    connect(calcThread,SIGNAL(sendMessage(QString)),currentStatusLabel,SLOT(setText(QString)));

    connect(startStopButton,SIGNAL(clicked()),this,SLOT(onStartStopButtonClicked()));

}

MainWindow::~MainWindow()
{

    delete calcThread;
}


void MainWindow::createWidgets()
{
    loadParametersPushButton = new QPushButton("Load");
    connect(loadParametersPushButton,SIGNAL(clicked()),this,SLOT(onLoadParametersPushButtonClicked()));
    saveParametersPushButton = new QPushButton("Save");
    connect(saveParametersPushButton,SIGNAL(clicked()),this,SLOT(onSaveParamatersPushButtonClicked()));



    spinningSpeedLineEdit = new QLineEdit();
    spinningSpeedLineEdit->setAlignment(Qt::AlignRight);

    nutationSpeedLineEdit = new QLineEdit();
    nutationSpeedLineEdit->setAlignment(Qt::AlignRight);

    distanceLineEdit = new QLineEdit();
    distanceLineEdit->setAlignment(Qt::AlignRight);

    magneticFieldLineEdit = new QLineEdit();
    magneticFieldLineEdit->setAlignment(Qt::AlignRight);

    gammaILineEdit = new QLineEdit();
    gammaILineEdit->setAlignment(Qt::AlignRight);

    gammaSLineEdit = new QLineEdit();
    gammaSLineEdit->setAlignment(Qt::AlignRight);

    angleIncrementLineEdit = new QLineEdit();
    angleIncrementLineEdit->setAlignment(Qt::AlignRight);

    nStepsLineEdit = new QLineEdit();
    nStepsLineEdit->setAlignment(Qt::AlignRight);

    nObsLineEdit = new QLineEdit();
    nObsLineEdit->setAlignment(Qt::AlignRight);

    alLineEdit = new QLineEdit();
    alLineEdit->setAlignment(Qt::AlignRight);


    outputFileNameLineEdit = new QLineEdit();
    outputFileNameLineEdit->setAlignment(Qt::AlignRight);

    outputFileNamePushButton = new QPushButton("...");
    connect(outputFileNamePushButton,SIGNAL(clicked()),this,SLOT(onOutputFileNamePushButtonClicked()));

    startStopButton = new QPushButton("Start");
    startStopButton->setFixedSize(80,80);

    currentStatusLabel = new QLabel(tr("Current Status:"));
}

void MainWindow::createPanel()
{

    QWidget *widget1 = new QWidget(this);
    setCentralWidget(widget1);

    QVBoxLayout *vLayout1 = new QVBoxLayout(widget1);

    QHBoxLayout *hLayout0 = new QHBoxLayout;
    hLayout0->addWidget(new QLabel(tr("Parameters")));
    hLayout0->addWidget(loadParametersPushButton);
    hLayout0->addWidget(saveParametersPushButton);


    QGridLayout *gLayout1 = new QGridLayout;

    gLayout1->addWidget(new QLabel(tr("Magnetic field")),   0,0,1,1);
    gLayout1->addWidget(magneticFieldLineEdit,              0,1,1,1);
    gLayout1->addWidget(new QLabel(tr("T")),                0,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Gyromagnetic ratio (S) ")), 1,0,1,1);
    gLayout1->addWidget(gammaSLineEdit,                            1,1,1,1);
    gLayout1->addWidget(new QLabel(tr("MHz/T")),                   1,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Gyromagnetic ratio (I) ")), 2,0,1,1);
    gLayout1->addWidget(gammaILineEdit,                            2,1,1,1);
    gLayout1->addWidget(new QLabel(tr("MHz/T")),                   2,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Distance between I and S")), 3,0,1,1);
    gLayout1->addWidget(distanceLineEdit,                           3,1,1,1);
    gLayout1->addWidget(new QLabel(tr("angstrom")),                 3,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Nutation speed")), 4,0,1,1);
    gLayout1->addWidget(nutationSpeedLineEdit,            4,1,1,1);
    gLayout1->addWidget(new QLabel(tr("kHz")),            4,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Spinning speed")), 5,0,1,1);
    gLayout1->addWidget(spinningSpeedLineEdit,            5,1,1,1);
    gLayout1->addWidget(new QLabel(tr("kHz")),            5,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Number of steps / rotation")), 6,0,1,1);
    gLayout1->addWidget(nStepsLineEdit,                               6,1,1,1);

    gLayout1->addWidget(new QLabel(tr("Number of steps / sampling")), 7,0,1,1);
    gLayout1->addWidget(nObsLineEdit,                                 7,1,1,1);

    gLayout1->addWidget(new QLabel(tr("Number of data points")), 8,0,1,1);
    gLayout1->addWidget(alLineEdit,                              8,1,1,1);

    gLayout1->addWidget(new QLabel(tr("Angle increment")), 9,0,1,1);
    gLayout1->addWidget(angleIncrementLineEdit,            9,1,1,1);
    gLayout1->addWidget(new QLabel(tr("deg")),             9,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Output file")),    10,0,1,1);
    gLayout1->addWidget(outputFileNameLineEdit,           10,1,1,1);
    gLayout1->addWidget(outputFileNamePushButton,         10,2,1,1);

    vLayout1->addLayout(hLayout0);
    vLayout1->addLayout(gLayout1);

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(startStopButton);
    hLayout1->addWidget(currentStatusLabel);

    vLayout1->addLayout(hLayout1);
    vLayout1->addStretch();


}


void MainWindow::onLoadParametersPushButtonClicked()
{
  QString fileName = QFileDialog::getOpenFileName(this,tr("Open parameter file"));
  if (fileName.isNull()) return;

  QSettings settings(fileName, QSettings::IniFormat);
  settings.beginGroup("r3");
    magneticFieldLineEdit->setText(settings.value("Magnetic field","").toString());
    gammaSLineEdit->setText(settings.value("Gyromagnetic ratio (S)","").toString());
    gammaILineEdit->setText(settings.value("Gyromagnetic ratio (I)","").toString());
    distanceLineEdit->setText(settings.value("Distance","").toString());
    spinningSpeedLineEdit->setText(settings.value("Spinning speed","").toString());
    nutationSpeedLineEdit->setText(settings.value("Nutation speed","").toString());
    angleIncrementLineEdit->setText(settings.value("Angle increment","").toString());
    outputFileNameLineEdit->setText(settings.value("Output filename","").toString());
    nStepsLineEdit->setText(settings.value("nSteps","").toString());
    nObsLineEdit->setText(settings.value("nObs","").toString());
    alLineEdit->setText(settings.value("al","").toString());
  settings.endGroup();

}

void MainWindow::onSaveParamatersPushButtonClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save parameter file"));
    if (fileName.isNull()) return;

    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup("r3");
      settings.setValue("Magnetic field", magneticFieldLineEdit->text());
      settings.setValue("Gyromagnetic ratio (S)", gammaSLineEdit->text());
      settings.setValue("Gyromagnetic ratio (I)", gammaILineEdit->text());
      settings.setValue("Distance", distanceLineEdit->text());
      settings.setValue("Spinning speed", spinningSpeedLineEdit->text());
      settings.setValue("Nutation speed", nutationSpeedLineEdit->text());
      settings.setValue("Angle increment", angleIncrementLineEdit->text());
      settings.setValue("Output filename", outputFileNameLineEdit->text());
      settings.setValue("nSteps", nStepsLineEdit->text());
      settings.setValue("nObs", nObsLineEdit->text());
      settings.setValue("al", alLineEdit->text());
    settings.endGroup();

    settings.sync();

}

bool MainWindow::setupParams()
{
    QString qs;
    double d;
    int k;
    bool ok;
//    const double twoPI=2.0*3.1415926535;

    qs=magneticFieldLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setMagneticField(d);}

    qs=gammaSLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setGammaS(d);}

    qs=gammaILineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setGammaI(d);}

    qs=distanceLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setDistance(d);}

    qs=spinningSpeedLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setSpinningSpeed(d);}

    qs=nutationSpeedLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setNutationSpeed(d);}

    qs=angleIncrementLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setAngleIncrement(d);}

    qs=nStepsLineEdit->text();
    k=qs.toInt(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setNSteps(k);}

    qs=nObsLineEdit->text();
    k=qs.toInt(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setNObs(k);}

    qs=alLineEdit->text();
    k=qs.toInt(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setAl(k);}

    calcThread->setFileName(outputFileNameLineEdit->text());

  return true;
}

void MainWindow::onOutputFileNamePushButtonClicked()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Output file"));
  if(filename.isNull()) return;

  outputFileNameLineEdit->setText(filename);

}

void MainWindow::onStartStopButtonClicked()
{

  if(!calcInProgress)
  {

    if(!setupParams()) return;
    startStopButton->setText("Stop");
    calcInProgress=true;
    calcThread->doCalc();
  }
  else
  {

    calcThread->stop();
    startStopButton->setText("Start");
    calcInProgress=false;
  }
}
