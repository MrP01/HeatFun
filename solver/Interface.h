#include "Solver.h"
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>

#define STEPS_PER_FRAME 4        // number of timesteps per frame
#define FRAMES_PER_MEASUREMENT 2 // how often we measure
#define STEPS_PER_MEASUREMENT (STEPS_PER_FRAME * FRAMES_PER_MEASUREMENT)
#define MEASUREMENTS_IN_ENERGY_PLOT 600

class HeatDemonstrator : public HeatSolver, public QMainWindow {
 private:
  QChart *temperatureChart = new QChart();
  QLineSeries *temperatureSeries = new QLineSeries();
  QScatterSeries *chebpointSeries = new QScatterSeries();

  QPushButton *stepBtn = new QPushButton("Step");
  QPushButton *controlBtn = new QPushButton("Start");
  QPushButton *reinitBtn = new QPushButton("Re-init");
  QPushButton *exportBtn = new QPushButton("Export");
  QLabel *statsLabel = new QLabel(QString("Heat"));

  size_t _step = 0;
  size_t _start_step = 0;
  size_t _timerId;
  void step();
  void timerEvent(QTimerEvent *event);

  // very important:
  void setTheme(QChart::ChartTheme theme);

 public:
  HeatDemonstrator() = default;
  void buildUI();
};
