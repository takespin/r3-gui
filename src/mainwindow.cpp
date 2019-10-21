#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QCoreApplication>
#include "math.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // By default, the path to the parameter file and the data file are set to the application directory path.
    // Later on, we will overwrite the path by the last used directory (in loadSettings)
    setParameterFilePath(QCoreApplication::applicationDirPath());
    setDataFilePath(QCoreApplication::applicationDirPath());

    calcInProgress=false;
    FID_2D = new TFID_2D;
    FID_2D->setAl(8192);
    FID_2D->FID.append(new TFID(FID_2D->al()));
    FID_2D->FID.last()->setEmpty(false);


    resize(QSize(400,600));
    createWidgets();
    createPanel();

    loadSettings();


    calcThread = new calcr3Thread;
    calcThread->setFID_2D(FID_2D);

    connect(calcThread,SIGNAL(calcComplete()),this,SLOT(onStartStopButtonClicked()));
    connect(calcThread,SIGNAL(sendMessage(QString)),currentStatusLabel,SLOT(setText(QString)));

    connect(startStopButton,SIGNAL(clicked()),this,SLOT(onStartStopButtonClicked()));
    connect(calcThread,SIGNAL(dataUpdated()), plotters, SLOT(update()));
    connect(calcThread,SIGNAL(xRangeUpdateRequest(int)), this, SLOT(xRangeUpdate(int)));


}

MainWindow::~MainWindow()
{
    // Before quitting the application, we save settings for the next run.
    saveSettings();
    delete calcThread;
}

void MainWindow::saveSettings()
{
    QString iniFileName=QCoreApplication::applicationDirPath()+"/settings.ini";
    // Note that Qt automatically converts "/" to "\" in Windows.

    QSettings settings(iniFileName, QSettings::IniFormat);
    settings.beginGroup("MainWindow");
//      settings.setValue("size", size());
      settings.setValue("position", pos());
    settings.endGroup();

    settings.beginGroup("Plotter");
      settings.setValue("size", plotters->size());
      settings.setValue("position", plotters->pos());
    settings.endGroup();

    settings.beginGroup("Paths");
      settings.setValue("parameterFilePath", parameterFilePath());
      settings.setValue("dataFilePath", dataFilePath());
    settings.endGroup();

    settings.sync();

}

void MainWindow::loadSettings()
{
    QString iniFileName=QCoreApplication::applicationDirPath()+"/settings.ini";
    if(!QFile::exists(iniFileName)) return;
    QSettings settings(iniFileName, QSettings::IniFormat);

    settings.beginGroup("MainWindow");
//      resize(settings.value("size", QSize(1000, 600)).toSize());
      move(settings.value("position", QPoint(100, 100)).toPoint());
    settings.endGroup();

    settings.beginGroup("Plotter");
      plotters->move(settings.value("position",QPoint(100,100)).toPoint());
      plotters->resize(settings.value("size",QSize(600,400)).toSize());
    settings.endGroup();

    settings.beginGroup("Paths");
      setParameterFilePath(settings.value("parameterFilePath",QCoreApplication::applicationDirPath()).toString());
      setDataFilePath(settings.value("dataFilePath",QCoreApplication::applicationDirPath()).toString());
    settings.endGroup();

}

void MainWindow::createWidgets()
{
    loadParametersPushButton = new QPushButton("Load");
    connect(loadParametersPushButton,SIGNAL(clicked()),this,SLOT(onLoadParametersPushButtonClicked()));
    saveParametersPushButton = new QPushButton("Save");
    connect(saveParametersPushButton,SIGNAL(clicked()),this,SLOT(onSaveParamatersPushButtonClicked()));


    isoILineEdit = new QLineEdit();
    isoILineEdit->setAlignment(Qt::AlignRight);

    isoSLineEdit = new QLineEdit();
    isoSLineEdit->setAlignment(Qt::AlignRight);

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

    apodizationWidthLineEdit = new QLineEdit();
    apodizationWidthLineEdit->setAlignment(Qt::AlignRight);

    outputFileNameLineEdit = new QLineEdit();
    outputFileNameLineEdit->setAlignment(Qt::AlignRight);

    outputFileNamePushButton = new QPushButton("...");
    connect(outputFileNamePushButton,SIGNAL(clicked()),this,SLOT(onOutputFileNamePushButtonClicked()));

    startStopButton = new QPushButton("Start");
    startStopButton->setFixedSize(80,80);

    currentStatusLabel = new QLabel(tr("Current Status:"));

    plotters = new TFIDPlotters;
    plotters->setFFTEnabled(true);
    plotters->setDevicePixelRatio(devicePixelRatio());
    plotters->setBackgroundColor0(QColor("lavender"));
    plotters->setBackgroundColor1(QColor("white"));

    plotters->show();
    plotters->raise();
    plotters->setFID2D(FID_2D);
    plotters->update();
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

//    gLayout1->addWidget(new QLabel(tr("Magnetic field")),   0,0,1,1);
//    gLayout1->addWidget(magneticFieldLineEdit,              0,1,1,1);
//    gLayout1->addWidget(new QLabel(tr("T")),                0,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Gyromagnetic ratio (S) ")), 0,0,1,1);
    gLayout1->addWidget(gammaSLineEdit,                            0,1,1,1);
    gLayout1->addWidget(new QLabel(tr("MHz/T")),                   0,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Gyromagnetic ratio (I) ")), 1,0,1,1);
    gLayout1->addWidget(gammaILineEdit,                            1,1,1,1);
    gLayout1->addWidget(new QLabel(tr("MHz/T")),                   1,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Isotropic Shift (S) ")), 2,0,1,1);
    gLayout1->addWidget(isoSLineEdit,                            2,1,1,1);
    gLayout1->addWidget(new QLabel(tr("Hz")),                   2,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Isotropic Shift (I) ")), 3,0,1,1);
    gLayout1->addWidget(isoILineEdit,                            3,1,1,1);
    gLayout1->addWidget(new QLabel(tr("Hz")),                   3,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Distance between I and S")), 4,0,1,1);
    gLayout1->addWidget(distanceLineEdit,                           4,1,1,1);
    gLayout1->addWidget(new QLabel(tr("angstrom")),                 4,2,1,1);

//    gLayout1->addWidget(new QLabel(tr("Nutation speed")), 4,0,1,1);
//    gLayout1->addWidget(nutationSpeedLineEdit,            4,1,1,1);
//    gLayout1->addWidget(new QLabel(tr("kHz")),            4,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Spinning speed")), 5,0,1,1);
    gLayout1->addWidget(spinningSpeedLineEdit,            5,1,1,1);
    gLayout1->addWidget(new QLabel(tr("kHz")),            5,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Number of steps / rotation")), 6,0,1,1);
    gLayout1->addWidget(nStepsLineEdit,                               6,1,1,1);

    gLayout1->addWidget(new QLabel(tr("Number of steps / sampling")), 7,0,1,1);
    gLayout1->addWidget(nObsLineEdit,                                 7,1,1,1);

    gLayout1->addWidget(new QLabel(tr("Number of data points")), 8,0,1,1);
    gLayout1->addWidget(alLineEdit,                              8,1,1,1);

    gLayout1->addWidget(new QLabel(tr("Apodization width")), 9,0,1,1);
    gLayout1->addWidget(apodizationWidthLineEdit,            9,1,1,1);
    gLayout1->addWidget(new QLabel(tr("Hz")),                9,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Angle increment")), 10,0,1,1);
    gLayout1->addWidget(angleIncrementLineEdit,            10,1,1,1);
    gLayout1->addWidget(new QLabel(tr("deg")),             10,2,1,1);

    gLayout1->addWidget(new QLabel(tr("Output file")),    11,0,1,1);
    gLayout1->addWidget(outputFileNameLineEdit,           11,1,1,1);
    gLayout1->addWidget(outputFileNamePushButton,         11,2,1,1);

    vLayout1->addWidget(new QLabel(tr("R2 Simulation")));
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

  QString fileName = QFileDialog::getOpenFileName(this, tr("Open parameter file"), parameterFilePath());

  if (fileName.isNull()) return;

  QFileInfo fi(fileName);
  setParameterFilePath(fi.absolutePath());

  QSettings settings(fileName, QSettings::IniFormat);
  settings.beginGroup("r2");
//    magneticFieldLineEdit->setText(settings.value("Magnetic field","").toString());
    gammaSLineEdit->setText(settings.value("Gyromagnetic ratio (S)","").toString());
    gammaILineEdit->setText(settings.value("Gyromagnetic ratio (I)","").toString());
    isoSLineEdit->setText(settings.value("Isotropic shift (S)","").toString());
    isoILineEdit->setText(settings.value("Isotropic shift (I)","").toString());
    distanceLineEdit->setText(settings.value("Distance","").toString());
    spinningSpeedLineEdit->setText(settings.value("Spinning speed","").toString());
    nutationSpeedLineEdit->setText(settings.value("Nutation speed","").toString());
    angleIncrementLineEdit->setText(settings.value("Angle increment","").toString());
    outputFileNameLineEdit->setText(settings.value("Output filename","").toString());
    nStepsLineEdit->setText(settings.value("nSteps","").toString());
    nObsLineEdit->setText(settings.value("nObs","").toString());
    alLineEdit->setText(settings.value("al","").toString());
    apodizationWidthLineEdit->setText(settings.value("Apodization width","").toString());
  settings.endGroup();

}

void MainWindow::onSaveParamatersPushButtonClicked()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save parameter file"), parameterFilePath());
    if (fileName.isNull()) return;
    QFileInfo fi(fileName);
    setParameterFilePath(fi.absolutePath());

    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup("r2");
//      settings.setValue("Magnetic field", magneticFieldLineEdit->text());
      settings.setValue("Gyromagnetic ratio (S)", gammaSLineEdit->text());
      settings.setValue("Gyromagnetic ratio (I)", gammaILineEdit->text());
      settings.setValue("Isotropic shift (S)", isoSLineEdit->text());
      settings.setValue("Isotropic shift (I)", isoILineEdit->text());
      settings.setValue("Distance", distanceLineEdit->text());
      settings.setValue("Spinning speed", spinningSpeedLineEdit->text());
      settings.setValue("Nutation speed", nutationSpeedLineEdit->text());
      settings.setValue("Angle increment", angleIncrementLineEdit->text());
      settings.setValue("Output filename", outputFileNameLineEdit->text());
      settings.setValue("nSteps", nStepsLineEdit->text());
      settings.setValue("nObs", nObsLineEdit->text());
      settings.setValue("al", alLineEdit->text());
      settings.setValue("Apodization width", apodizationWidthLineEdit->text());
    settings.endGroup();

    settings.sync();

}

bool MainWindow::setupParams()
{
    QString qs;
    double d;
    int k;
    bool ok;

//    qs=magneticFieldLineEdit->text();
//    d=qs.toDouble(&ok);
//    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
//    else {calcThread->setMagneticField(d);}

    qs=gammaSLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setGammaS(d);}

    qs=gammaILineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setGammaI(d);}

    qs=isoSLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setIsoS(d);}

    qs=isoILineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setIsoI(d);}

    qs=distanceLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setDistance(d);}

    qs=spinningSpeedLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setSpinningSpeed(d);}

    qs=apodizationWidthLineEdit->text();
    d=qs.toDouble(&ok);
    if(!ok) {currentStatusLabel->setText("Invalid expression: " + qs); return false;}
    else {calcThread->setApodizationWidth(d);}

    //qs=nutationSpeedLineEdit->text();
    //d=qs.toDouble(&ok);
    //if(!ok) {currentStatusLabel->setText("Invalid expression: 7" + qs); return false;}
    //else {calcThread->setNutationSpeed(d);}

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
  QString fileName = QFileDialog::getSaveFileName(this, tr("Output file"));
  if(fileName.isNull()) return;

  QFileInfo fi(fileName);
  setDataFilePath(fi.absolutePath());

  outputFileNameLineEdit->setText(fileName);

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

void MainWindow::initializePlotter()
{
    for(int k=0; k<plotters->FIDPlotters.size(); k++)
    {
        plotters->FIDPlotters[k]->plotter->xini=0;
        plotters->FIDPlotters[k]->plotter->xfin=FID_2D->al()-1;
        plotters->FIDPlotters[k]->plotter->setScale(1/FID_2D->FID[0]->abs->absMax());
        plotters->FIDPlotters[k]->FIDSelectSpinBox->setMinimum(1);
        plotters->FIDPlotters[k]->FIDSelectSpinBox->setMaximum(FID_2D->FID.size());

        plotters->FIDPlotters[k]->xFullRangePlot();
    }

}

void MainWindow::xRangeUpdate(int k)
{
    for(int k=0; k<plotters->FIDPlotters.size(); k++)
    {

        plotters->FIDPlotters[k]->xFullRangePlot();

    }

}
