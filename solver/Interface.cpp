#include "Interface.h"

#define N_LINSPACE_POINTS_TO_PLOT 500
static QChart::ChartTheme THEMES[5] = {QChart::ChartThemeLight, QChart::ChartThemeDark, QChart::ChartThemeBlueCerulean,
    QChart::ChartThemeBrownSand, QChart::ChartThemeBlueIcy};

void HeatDemonstrator::setupExpression(std::string expression) {
  plotAndLoadU0Expression(expression);
  try {
    setup(evaluateExpression(expression, TschebFun::modifiedChebpoints(orderEdit->value())));
    plotCurrentU();
    if (showChebpoints->isChecked())
      plotChebpoints();
  } catch (mup::ParserError) {
    std::cout << "Could not parse expression" << std::endl;
  }
}

void HeatDemonstrator::plotAndLoadU0Expression(std::string expression) {
  Vector X = xt::linspace(-1.0, 1.0, N_LINSPACE_POINTS_TO_PLOT);
  try {
    Vector Y = evaluateExpression(expression, X);
    plotXYSeries(u0Series, X, Y);
  } catch (mup::ParserError) {
    std::cout << "Could not parse expression" << std::endl;
  }
}

void HeatDemonstrator::step() {
  iterate();
  plotCurrentU();
}

void HeatDemonstrator::plotCurrentU() {
  Vector X = xt::linspace(-1.0, 1.0, N_LINSPACE_POINTS_TO_PLOT);
  Vector Y = currentU.evaluateOn(X);
  plotXYSeries(temperatureSeries, X, Y);
}

void HeatDemonstrator::plotChebpoints() {
  Vector X = TschebFun::modifiedChebpoints(currentU.order());
  Vector Y = currentU.evaluateOn(X);
  plotXYSeries(chebpointSeries, X, Y);
}

void HeatDemonstrator::plotXYSeries(QXYSeries *series, Vector X, Vector Y) {
  series->clear();
  for (size_t i = 0; i < X.size(); i++)
    series->append(X[i], Y[i]);
  temperatureChart->axes(Qt::Vertical).first()->setRange(xt::amin(Y)() - 0.1, xt::amax(Y)() + 0.1);
}

std::string HeatDemonstrator::getExpression() {
  std::string expr = expressionLineEdit->text().toStdString();
  return expr.size() > 0 ? expr : "sin((4*x)^2) + sin(4*x)^2";
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
  connect(expressionLineEdit, &QLineEdit::returnPressed, [=, this]() { setupExpression(getExpression()); });
  connect(orderEdit, &QSpinBox::valueChanged, [=, this]() { setupExpression(getExpression()); });

  connect(showChebpoints, &QCheckBox::stateChanged, [=, this]() {
    if (showChebpoints->isChecked()) {
      if (chebpointSeries->points().size() == 0)
        plotChebpoints();
      chebpointSeries->setVisible(true);
    } else
      chebpointSeries->setVisible(false);
  });
  orderEdit->setMinimum(1);
  orderEdit->setMaximum(2000);
  orderEdit->setValue(30);
  orderEdit->setMaximumWidth(200);

  connect(differentiationBtn, &QPushButton::clicked, [=, this]() {
    currentU = currentU.derivative();
    plotCurrentU();
  });
  connect(stepBtn, &QPushButton::clicked, [=, this]() { step(); });
  connect(controlBtn, &QPushButton::clicked, [=, this]() {
    if (controlBtn->text() == "Start") {
      _timerId = startTimer(100);
      _start_step = _step;
      controlBtn->setText("Stop");
    } else {
      killTimer(_timerId);
      controlBtn->setText("Start");
    }
  });

  auto mainWidget = new QWidget(this);
  auto mainLayout = new QGridLayout(mainWidget);
  auto topLayout = new QHBoxLayout();
  topLayout->addWidget(expressionLineEdit);
  topLayout->addWidget(orderEdit);
  auto buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(controlBtn);
  buttonLayout->addWidget(stepBtn);
  buttonLayout->addWidget(differentiationBtn);
  buttonLayout->addWidget(reinitBtn);
  buttonLayout->addWidget(exportBtn);
  buttonLayout->addWidget(createThemeChooser());
  buttonLayout->addWidget(showChebpoints);
  mainLayout->addLayout(topLayout, 0, 0);
  mainLayout->addWidget(temperatureView, 1, 0);
  mainLayout->addLayout(buttonLayout, 2, 0);
  setCentralWidget(mainWidget);
  setWindowTitle("Spectral Heat Equation Solver");

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
  connect(themeBox, &QComboBox::currentIndexChanged, [=, this]() { setTheme(THEMES[themeBox->currentIndex()]); });
  return themeBox;
}

void HeatDemonstrator::setTheme(QChart::ChartTheme theme) {
  std::cout << "Theme set to " << theme << std::endl;
  temperatureChart->setTheme(theme);
}
