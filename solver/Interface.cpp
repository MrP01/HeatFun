#include "Interface.h"

#define N_LINSPACE_POINTS_TO_PLOT 500

void HeatDemonstrator::setupExpression(std::string expression) {
  plotAndLoadU0Expression(expression);
  setup(evaluateExpression(expression, ChebFun::chebpoints(120)));
  plotCurrentU();
  if (showChebpoints->isChecked())
    plotChebpoints();
}

void HeatDemonstrator::plotAndLoadU0Expression(std::string expression) {
  Vector X = xt::linspace(-1.0, 1.0, N_LINSPACE_POINTS_TO_PLOT);
  try {
    Vector Y = evaluateExpression(expression, X);
    plotXYSeries(u0Series, X, Y);
    temperatureChart->axes(Qt::Vertical).first()->setRange(xt::amin(Y)() - 0.1, xt::amax(Y)() + 0.1);
  } catch (mup::ParserError) {
    std::cout << "Could not parse expression" << std::endl;
  }
}

void HeatDemonstrator::step() {}

void HeatDemonstrator::timerEvent(QTimerEvent *event) {}

void HeatDemonstrator::plotChebpoints() {
  Vector X = ChebFun::chebpoints(currentU.degree());
  Vector Y = currentU.evaluate(X);
  plotXYSeries(chebpointSeries, X, Y);
}

void HeatDemonstrator::plotCurrentU() {
  Vector X = xt::linspace(-1, 1, N_LINSPACE_POINTS_TO_PLOT);
  Vector Y = currentU.evaluate(X);
  plotXYSeries(temperatureSeries, X, Y);
}

void HeatDemonstrator::plotXYSeries(QXYSeries *series, Vector X, Vector Y) {
  series->clear();
  for (size_t i = 0; i < N_LINSPACE_POINTS_TO_PLOT; i++)
    series->append(X[i], Y[i]);
}

void HeatDemonstrator::buildUI() {
  temperatureSeries->setName("Temperature u(x)");
  chebpointSeries->setName("Chebpoints");
  u0Series->setName("u_0(x)");
  temperatureChart->addSeries(temperatureSeries);
  temperatureChart->addSeries(chebpointSeries);
  temperatureChart->addSeries(u0Series);
  temperatureChart->createDefaultAxes();
  temperatureChart->axes(Qt::Horizontal).first()->setRange(-1, 1);
  QChartView *temperatureView = new QChartView(temperatureChart);

  expressionLineEdit->setPlaceholderText("Enter Expression for u_0(x)");
  connect(expressionLineEdit, &QLineEdit::returnPressed, [=, this]() {
    std::string expression = expressionLineEdit->text().toStdString();
    setupExpression(expression);
  });

  connect(showChebpoints, &QCheckBox::stateChanged, [=, this]() {
    if (showChebpoints->isChecked()) {
      if (chebpointSeries->points().size() == 0)
        plotChebpoints();
      chebpointSeries->setVisible(true);
    } else
      chebpointSeries->setVisible(false);
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
  buttonLayout->addWidget(createThemeChooser());
  buttonLayout->addWidget(showChebpoints);
  mainLayout->addLayout(buttonLayout, 2, 0);
  setCentralWidget(mainWidget);
  setWindowTitle("Heat Equation Solver");

  QShortcut *closeShortcut = new QShortcut(Qt::CTRL | Qt::Key_W, this);
  QObject::connect(closeShortcut, &QShortcut::activated, this, [=, this]() { close(); });
}

QComboBox *HeatDemonstrator::createThemeChooser() {
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
  return themeBox;
}

void HeatDemonstrator::setTheme(QChart::ChartTheme theme) { temperatureChart->setTheme(theme); }
