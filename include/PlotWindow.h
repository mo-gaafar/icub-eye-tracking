#pragma once

#include <QMainWindow>
#include <QTimer>
#include <deque>
#include "qcustomplot.h"

class PlotWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    void addDataPoint(double errorX, double errorY, 
                     double eyeX, double eyeY,
                     double neckPitch, double neckYaw);

private slots:
    void updatePlot();

private:
    QCustomPlot *errorPlot;
    QCustomPlot *positionPlot;  // Combined eye and neck positions
    QTimer dataTimer;
    
    static const int MAX_POINTS = 200;  // Maximum points to show
    
    // Data storage
    std::deque<double> timePoints;
    std::deque<double> errorXData;
    std::deque<double> errorYData;
    std::deque<double> eyeXData;
    std::deque<double> eyeYData;
    std::deque<double> neckPitchData;
    std::deque<double> neckYawData;
    
    double startTime;
    
    void setupErrorPlot();
    void setupPositionPlot();  // Combined setup for eye and neck
};
