#include "Interface.h"
#include <QComboBox>

#define INITIAL_EXPRESSION "sin((4*x)^2) + sin(4*x)^2"
#define N_LINSPACE_POINTS_TO_PLOT 500

void HeatDemonstrator::buildUI() {
  temperatureSeries->setName("Temperature");
  chebpointSeries->setName("Chebpoints");
  temperatureChart->addSeries(temperatureSeries);
  temperatureChart->addSeries(chebpointSeries);
  temperatureChart->createDefaultAxes();
  temperatureChart->axes(Qt::Horizontal).first()->setRange(-1, 1);
  QChartView *temperatureView = new QChartView(temperatureChart);

  expressionLineEdit->setPlaceholderText("Enter Expression for u_0(x)");
  connect(expressionLineEdit, &QLineEdit::returnPressed,
      [=, this]() { plotAndLoadExpression(expressionLineEdit->text().toStdString()); });
  plotAndLoadExpression(INITIAL_EXPRESSION);
  plotChebpoints(120);

  QComboBox *themeBox = new QComboBox();
  themeBox->addItem("Light");
  themeBox->addItem("Dark");
  themeBox->addItem("Cerulean Blue");
  themeBox->addItem("Brown Sand");
  themeBox->addItem("Icy Blue");
  connect(themeBox, &QComboBox::currentIndexChanged, [=, this]() {
    switch (themeBox->currentIndex()) {
    case 0:
      setTheme(QChart::ChartThemeLight);
    case 1:
      setTheme(QChart::ChartThemeDark);
    case 2:
      setTheme(QChart::ChartThemeBlueCerulean);
    case 3:
      setTheme(QChart::ChartThemeBrownSand);
    case 4:
      setTheme(QChart::ChartThemeBlueIcy);
    default:
      break;
    }
  });

  auto mainWidget = new QWidget(this);
  auto mainLayout = new QGridLayout(mainWidget);
  mainLayout->addWidget(expressionLineEdit, 0, 0);
  mainLayout->addWidget(temperatureView, 1, 0);
  auto buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(controlBtn);
  buttonLayout->addWidget(stepBtn);
  buttonLayout->addWidget(reinitBtn);
  buttonLayout->addWidget(exportBtn);
  buttonLayout->addWidget(themeBox);
  mainLayout->addLayout(buttonLayout, 2, 0);
  setCentralWidget(mainWidget);
  setWindowTitle("Heat Equation Solver");

  QShortcut *closeShortcut = new QShortcut(Qt::CTRL | Qt::Key_W, this);
  QObject::connect(closeShortcut, &QShortcut::activated, this, [=, this]() { close(); });
}

void HeatDemonstrator::plotAndLoadExpression(std::string expression) {
  temperatureSeries->clear();
  Vector X = xt::linspace(-1.0, 1.0, N_LINSPACE_POINTS_TO_PLOT);
  Vector Y = evaluateExpression(expression, X);
  try {
    for (size_t i = 0; i < N_LINSPACE_POINTS_TO_PLOT; i++)
      temperatureSeries->append(X[i], Y[i]);
    temperatureChart->axes(Qt::Vertical).first()->setRange(xt::amin(Y)() - 0.1, xt::amax(Y)() + 0.1);
  } catch (mup::ParserError) {
    std::cout << "Could not parse expression" << std::endl;
  }
}

void HeatDemonstrator::step() {}

void HeatDemonstrator::timerEvent(QTimerEvent *event) {}

void HeatDemonstrator::plotChebpoints(size_t N) {
  chebpointSeries->clear();
  Vector chebpoints = ChebFun::chebpoints(N);
  for (auto iter = chebpoints.begin(); iter != chebpoints.end(); iter++)
    chebpointSeries->append(*iter, evaluateExpression(INITIAL_EXPRESSION, *iter)[0]);
}

void HeatDemonstrator::setTheme(QChart::ChartTheme theme) {}
