#pragma once

#include "Solver.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
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
  QLineSeries *u0Series = new QLineSeries();
  QLineSeries *temperatureSeries = new QLineSeries();
  QScatterSeries *chebpointSeries = new QScatterSeries();

  QLineEdit *expressionLineEdit = new QLineEdit();
  QSpinBox *orderEdit = new QSpinBox();
  QPushButton *controlBtn = new QPushButton("Start");
  QPushButton *stepBtn = new QPushButton("Step");
  QPushButton *differentiationBtn = new QPushButton("Differentiate");
  QPushButton *reinitBtn = new QPushButton("Re-init");
  QPushButton *exportBtn = new QPushButton("Export");
  QCheckBox *showChebpoints = new QCheckBox("Show Chebpoints");
  QLabel *statsLabel = new QLabel("Heat");

  size_t _step = 0;
  size_t _start_step = 0;
  size_t _timerId;
  void step();
  void timerEvent(QTimerEvent *event) { step(); };

  void plotChebpoints();
  void plotCurrentU();
  void plotXYSeries(QXYSeries *series, Vector X, Vector Y, bool adaptYAxis = false);

  // very important:
  QComboBox *createThemeChooser();
  void setTheme(QChart::ChartTheme theme);

 public:
  HeatDemonstrator() = default;
  void buildUI();
  void setupExpression(std::string expression);
  void plotAndLoadU0Expression(std::string expression);
  std::string getExpression();
};
