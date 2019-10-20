#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include "calcr3.h"
#include "plotter.h"
#include "fid.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    calcr3Thread *calcThread;

public slots:
    void initializePlotter();
    void xRangeUpdate(int k);

private slots:
    void onLoadParametersPushButtonClicked();
    void onSaveParamatersPushButtonClicked();
    void onOutputFileNamePushButtonClicked();
    void onStartStopButtonClicked();

private:
    void createWidgets();
    void createPanel();
    void loadParameters();
    void saveParameters();
    bool calcInProgress;
    bool setupParams();

    TFIDPlotters *plotters;
    TFID_2D *FID_2D;

    QPushButton *loadParametersPushButton;
    QPushButton *saveParametersPushButton;

    QLineEdit *isoILineEdit;
    QLineEdit *isoSLineEdit;
    QLineEdit *spinningSpeedLineEdit;
    QLineEdit *nutationSpeedLineEdit;
    QLineEdit *distanceLineEdit;
    QLineEdit *magneticFieldLineEdit;
    QLineEdit *gammaILineEdit;
    QLineEdit *gammaSLineEdit;
    QLineEdit *angleIncrementLineEdit;
    QLineEdit *outputFileNameLineEdit;
    QLineEdit *nStepsLineEdit;
    QLineEdit *alLineEdit;
    QLineEdit *nObsLineEdit;
    QPushButton *outputFileNamePushButton;
    QPushButton *startStopButton;
    QLabel *currentStatusLabel;

};



#endif // MAINWINDOW_H
