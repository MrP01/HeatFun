#include "Interface.h"
#include <QComboBox>

void HeatDemonstrator::buildUI() {
  temperatureSeries->setName("Temperature");
  temperatureChart->addSeries(temperatureSeries);
  temperatureChart->createDefaultAxes();
  temperatureChart->axes(Qt::Horizontal).first()->setRange(-1, 1);
  QChartView *temperatureView = new QChartView(temperatureChart);

  expressionLineEdit->setPlaceholderText("Enter Expression for u_0(x)");
  connect(expressionLineEdit, &QLineEdit::returnPressed,
      [=, this]() { plotAndLoadExpression(expressionLineEdit->text().toStdString()); });

  QComboBox *themeBox = new QComboBox();
  themeBox->addItem("Light");
  themeBox->addItem("Dark");
  themeBox->addItem("Cerulean Blue");
  themeBox->addItem("Brown Sand");
  themeBox->addItem("Icy Blue");
  connect(themeBox, &QComboBox::currentIndexChanged, [=, this]() {
    std::cout << themeBox->currentIndex();
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
  mup::Value variable;
  mup::ParserX parser;
  parser.DefineVar("x", mup::Variable(&variable));
  parser.SetExpr(expression);

  temperatureSeries->clear();
  double y, yMin = 0, yMax = 0;
  try {
    for (double x = -1.0; x <= 1.0; x += 0.005) {
      variable = x;
      y = parser.Eval().GetFloat();
      temperatureSeries->append(x, y);
      if (y < yMin)
        yMin = y;
      if (y > yMax)
        yMax = y;
    }
    temperatureChart->axes(Qt::Vertical).first()->setRange(yMin - 0.1, yMax + 0.1);
  } catch (mup::ParserError) {
    std::cout << "Could not parse expression" << std::endl;
  }
}

void HeatDemonstrator::step() {}

void HeatDemonstrator::timerEvent(QTimerEvent *event) {}

void HeatDemonstrator::setTheme(QChart::ChartTheme theme) {}
