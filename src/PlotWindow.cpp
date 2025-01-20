#include "PlotWindow.h"
#include <QVBoxLayout>
#include <QWidget>

PlotWindow::PlotWindow(QWidget *parent) 
    : QMainWindow(parent), startTime(0) {
    
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    // Create plots
    errorPlot = new QCustomPlot(this);
    positionPlot = new QCustomPlot(this);
    
    // Add plots to layout
    layout->addWidget(errorPlot);
    layout->addWidget(positionPlot);
    
    // Setup plots
    setupErrorPlot();
    setupPositionPlot();
    
    // Set central widget
    setCentralWidget(centralWidget);
    resize(800, 600);
    
    // Setup timer for real-time updates
    connect(&dataTimer, &QTimer::timeout, this, &PlotWindow::updatePlot);
    dataTimer.start(50);  // Update every 50ms
    
    startTime = QDateTime::currentMSecsSinceEpoch() / 1000.0;
}

void PlotWindow::setupErrorPlot() {
    errorPlot->addGraph(); // Error X
    errorPlot->addGraph(); // Error Y
    
    // Set names
    errorPlot->graph(0)->setName("Error X");
    errorPlot->graph(1)->setName("Error Y");
    
    // Set colors
    errorPlot->graph(0)->setPen(QPen(Qt::red));
    errorPlot->graph(1)->setPen(QPen(Qt::blue));
    
    // Set axes labels
    errorPlot->xAxis->setLabel("Time (s)");
    errorPlot->yAxis->setLabel("Error (pixels)");
    
    // Set axes ranges
    errorPlot->xAxis->setRange(0, 10);
    errorPlot->yAxis->setRange(-100, 100);
    
    // Show legend
    errorPlot->legend->setVisible(true);
    errorPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void PlotWindow::setupPositionPlot() {
    // Add graphs for eyes and neck
    positionPlot->addGraph(); // Eye Yaw
    positionPlot->addGraph(); // Eye Tilt
    positionPlot->addGraph(); // Neck Yaw
    positionPlot->addGraph(); // Neck Pitch
    
    // Set names
    positionPlot->graph(0)->setName("Eye Yaw");
    positionPlot->graph(1)->setName("Eye Tilt");
    positionPlot->graph(2)->setName("Neck Yaw");
    positionPlot->graph(3)->setName("Neck Pitch");
    
    // Set colors and styles
    positionPlot->graph(0)->setPen(QPen(QColor(255, 50, 50), 2));  // Bright red for eye yaw
    positionPlot->graph(1)->setPen(QPen(QColor(50, 50, 255), 2));  // Bright blue for eye tilt
    positionPlot->graph(2)->setPen(QPen(QColor(200, 50, 50), 2, Qt::DashLine));  // Dashed dark red for neck yaw
    positionPlot->graph(3)->setPen(QPen(QColor(50, 50, 200), 2, Qt::DashLine));  // Dashed dark blue for neck pitch
    
    // Set axes labels
    positionPlot->xAxis->setLabel("Time (s)");
    positionPlot->yAxis->setLabel("Position (degrees)");
    
    // Set axes ranges
    positionPlot->xAxis->setRange(0, 10);
    positionPlot->yAxis->setRange(-30, 30);
    
    // Show legend
    positionPlot->legend->setVisible(true);
    positionPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void PlotWindow::addDataPoint(double errorX, double errorY, 
                            double eyeX, double eyeY,
                            double neckPitch, double neckYaw) {
    double currentTime = QDateTime::currentMSecsSinceEpoch() / 1000.0 - startTime;
    
    // Add new data
    timePoints.push_back(currentTime);
    errorXData.push_back(errorX);
    errorYData.push_back(errorY);
    eyeXData.push_back(eyeX);
    eyeYData.push_back(eyeY);
    neckPitchData.push_back(neckPitch);
    neckYawData.push_back(neckYaw);
    
    // Remove old data if necessary
    if (timePoints.size() > MAX_POINTS) {
        timePoints.pop_front();
        errorXData.pop_front();
        errorYData.pop_front();
        eyeXData.pop_front();
        eyeYData.pop_front();
        neckPitchData.pop_front();
        neckYawData.pop_front();
    }
}

void PlotWindow::updatePlot() {
    if (timePoints.empty()) return;
    
    // Convert deques to QVector for plotting
    QVector<double> time = QVector<double>::fromStdVector(
        std::vector<double>(timePoints.begin(), timePoints.end()));
    
    // Update error plot
    errorPlot->graph(0)->setData(time, QVector<double>::fromStdVector(
        std::vector<double>(errorXData.begin(), errorXData.end())));
    errorPlot->graph(1)->setData(time, QVector<double>::fromStdVector(
        std::vector<double>(errorYData.begin(), errorYData.end())));
    
    // Update position plot
    positionPlot->graph(0)->setData(time, QVector<double>::fromStdVector(
        std::vector<double>(eyeXData.begin(), eyeXData.end())));
    positionPlot->graph(1)->setData(time, QVector<double>::fromStdVector(
        std::vector<double>(eyeYData.begin(), eyeYData.end())));
    positionPlot->graph(2)->setData(time, QVector<double>::fromStdVector(
        std::vector<double>(neckYawData.begin(), neckYawData.end())));
    positionPlot->graph(3)->setData(time, QVector<double>::fromStdVector(
        std::vector<double>(neckPitchData.begin(), neckPitchData.end())));
    
    // Auto-scale x axis to show all data
    double minTime = timePoints.front();
    double maxTime = timePoints.back();
    errorPlot->xAxis->setRange(minTime, maxTime + 1);
    positionPlot->xAxis->setRange(minTime, maxTime + 1);
    
    // Replot
    errorPlot->replot();
    positionPlot->replot();
}
