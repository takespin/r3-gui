#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include "calcr3.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    calcr3Thread *calcThread;


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
