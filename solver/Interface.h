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
#include <fstream>
#include <xtensor/xcsv.hpp>

#define STEPS_PER_FRAME 12 // number of timesteps per frame, TODO

class HeatDemonstrator : public HeatSolver, public QMainWindow {
 private:
  QChart *temperatureChart = new QChart();
  QLineSeries *u0Series = new QLineSeries();
  QLineSeries *temperatureSeries = new QLineSeries();
  QScatterSeries *chebpointSeries = new QScatterSeries();

  QLineEdit *expressionLineEdit = new QLineEdit();
  QSpinBox *orderEdit = new QSpinBox();
  QLineEdit *dtEdit = new QLineEdit();
  QLineEdit *goalDuEdit = new QLineEdit();
  QPushButton *controlBtn = new QPushButton("Start");
  QPushButton *stepBtn = new QPushButton("Step");
  QPushButton *differentiationBtn = new QPushButton("Differentiate");
  QPushButton *reinitBtn = new QPushButton("Reset");
  QPushButton *rescaleBtn = new QPushButton("Rescale");
  QPushButton *exportBtn = new QPushButton("Export");
  QCheckBox *showChebpoints = new QCheckBox("Show chebpoints");
  QCheckBox *adaptiveDtCheckBox = new QCheckBox("Adaptive time-step");
  QCheckBox *linearMultistepCheckBox = new QCheckBox("Linear multistep method");
  QLabel *statsLabel = new QLabel("Press 'Start' to get going!");

  size_t _step = 0;
  size_t _start_step = 0;
  size_t _timerId;
  void step();
  void timerEvent(QTimerEvent *event) { step(); };

  void plotChebpoints();
  void plotCurrentU(bool adaptYAxis = false);
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
