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

#define STEPS_PER_FRAME 20 // number of timesteps per frame, TODO
#define N_LINSPACE_POINTS_TO_PLOT 500
#define SIDE_PANEL_MAX_WIDTH 250

class HeatDemonstrator : public QMainWindow {
 protected:
  HeatSolver *_solver = nullptr;
  virtual HeatSolver *solver() { return _solver; }

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
  virtual void step();
  virtual void timerEvent(QTimerEvent *event) { step(); };

  virtual void plotChebpoints();
  virtual void plotCurrentU(bool adaptYAxis = false);
  virtual void plotXYSeries(QXYSeries *series, Vector X, Vector Y, bool adaptYAxis = false);

  // very important:
  QComboBox *createThemeChooser();
  void setTheme(QChart::ChartTheme theme);

 public:
  HeatDemonstrator() { _solver = new HeatSolver(); };
  virtual void buildUI();
  virtual void setupExpression(std::string expression);
  virtual void plotAndLoadU0Expression(std::string expression);
  virtual std::string getExpression();
};
