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

#define STEPS_PER_FRAME 4        // number of timesteps per frame
#define FRAMES_PER_MEASUREMENT 2 // how often we measure
#define STEPS_PER_MEASUREMENT (STEPS_PER_FRAME * FRAMES_PER_MEASUREMENT)
#define MEASUREMENTS_IN_ENERGY_PLOT 600

class BoxSimulator : public ParticleBox, public QMainWindow {
 private:
  QChart *energyChart = new QChart();
  QLineSeries *kineticEnergySeries = new QLineSeries();
  QLineSeries *potentialEnergySeries = new QLineSeries();
  QLineSeries *LJpotentialEnergySeries = new QLineSeries();
  QLineSeries *totalEnergySeries = new QLineSeries();

  QPushButton *stepBtn = new QPushButton("Step");
  QPushButton *controlBtn = new QPushButton("Start");
  QPushButton *reinitBtn = new QPushButton("Re-init");
  QPushButton *exportBtn = new QPushButton("Export");
  QLabel *statsLabel = new QLabel(QString("Heat"));

  size_t _step = 0;
  size_t _start_step = 0;
  double _energyMax = 0;
  size_t _timerId;
  void renderParticles();
  void updateHistograms();
  void measure();
  void step();
  void timerEvent(QTimerEvent *event);

  // very important:
  void setTheme(QChart::ChartTheme theme);

 public:
  BoxSimulator() = default;
  void buildUI();
};
