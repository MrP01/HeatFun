#include "Interface.h"

// TODO: potentially implement drag-and-drop functionality of u0 with the mouse

static QChart::ChartTheme THEMES[5] = {QChart::ChartThemeLight, QChart::ChartThemeDark, QChart::ChartThemeBlueCerulean,
    QChart::ChartThemeBrownSand, QChart::ChartThemeBlueIcy};

void HeatDemonstrator::setupExpression(std::string expression) {
  plotAndLoadU0Expression(expression);
  try {
    solver->setup(evaluateExpression(expression, TschebFun::modifiedChebpoints(orderEdit->value())));
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
    plotXYSeries(u0Series, X, Y, true);
  } catch (mup::ParserError) {
    std::cout << "Could not parse expression" << std::endl;
  }
}

void HeatDemonstrator::step() {
  for (size_t i = 0; i < STEPS_PER_FRAME; i++)
    solver->iterate();
  plotCurrentU();
  statsLabel->setText(QString("Current time: t = %1\nTime-step dt = %2").arg(solver->totalTime).arg(solver->dt));
}

void HeatDemonstrator::plotCurrentU(bool adaptYAxis) {
  Vector X = xt::linspace(-1.0, 1.0, N_LINSPACE_POINTS_TO_PLOT);
  Vector Y = solver->currentU.evaluateOn(X);
  plotXYSeries(temperatureSeries, X, Y, adaptYAxis);
}

void HeatDemonstrator::plotChebpoints() {
  Vector X = TschebFun::modifiedChebpoints(solver->currentU.order());
  Vector Y = solver->currentU.evaluateOn(X);
  plotXYSeries(chebpointSeries, X, Y);
}

void HeatDemonstrator::plotXYSeries(QXYSeries *series, Vector X, Vector Y, bool adaptYAxis) {
  series->clear();
  for (size_t i = 0; i < X.size(); i++)
    series->append(X[i], Y[i]);
  if (adaptYAxis)
    temperatureChart->axes(Qt::Vertical).first()->setRange(xt::amin(Y)() - 0.1, xt::amax(Y)() + 0.1);
}

std::string HeatDemonstrator::getExpression() {
  std::string expr = expressionLineEdit->text().toStdString();
  return expr.size() > 0 ? expr : "sin((4*x)^2) + sin(4*x)^2";
  // return expr.size() > 0 ? expr : "1";
}

void HeatDemonstrator::buildUI() {
  temperatureSeries->setName("Temperature u(x)");
  chebpointSeries->setName("Chebpoints");
  u0Series->setName("u_0(x)");
  u0Series->setColor(Qt::gray);
  temperatureChart->addSeries(temperatureSeries);
  temperatureChart->addSeries(chebpointSeries);
  temperatureChart->addSeries(u0Series);
  temperatureChart->createDefaultAxes();
  temperatureChart->axes(Qt::Horizontal).first()->setTitleText("Location x");
  temperatureChart->axes(Qt::Vertical).first()->setTitleText("Temperature u(x)");
  temperatureChart->axes(Qt::Horizontal).first()->setRange(-1.0, 1.0);
  QChartView *temperatureView = new QChartView(temperatureChart);

  QLabel *expressionLabel = new QLabel("&Initial condition u_0(x):", this);
  expressionLabel->setBuddy(expressionLineEdit);
  expressionLineEdit->setPlaceholderText("Enter expression for u_0(x)");
  expressionLineEdit->setMaximumWidth(SIDE_PANEL_MAX_WIDTH);
  connect(expressionLineEdit, &QLineEdit::returnPressed, [=, this]() { setupExpression(getExpression()); });
  connect(orderEdit, &QSpinBox::valueChanged, [=, this]() { setupExpression(getExpression()); });

  QLabel *orderLabel = new QLabel("&Order of interpolation:", this);
  orderLabel->setBuddy(orderEdit);
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
  orderEdit->setMaximumWidth(SIDE_PANEL_MAX_WIDTH);

  QLabel *dtLabel = new QLabel("&Time-step dt:", this);
  dtLabel->setBuddy(dtEdit);
  dtEdit->setPlaceholderText("dt");
  dtEdit->setText(QString::number(solver->dt, 'e'));
  dtEdit->setMaximumWidth(SIDE_PANEL_MAX_WIDTH);
  connect(dtEdit, &QLineEdit::returnPressed, [=, this]() {
    solver->dt = dtEdit->text().toDouble();
    std::cout << "Set dt = " << solver->dt << std::endl;
  });

  QLabel *goalDuLabel = new QLabel("&Adaptive goal du:", this);
  goalDuLabel->setBuddy(goalDuEdit);
  goalDuEdit->setPlaceholderText("du");
  goalDuEdit->setText(QString::number(solver->optimisations.adaptiveGoalDu, 'e'));
  goalDuEdit->setMaximumWidth(SIDE_PANEL_MAX_WIDTH);
  connect(dtEdit, &QLineEdit::returnPressed, [=, this]() {
    solver->optimisations.adaptiveGoalDu = goalDuEdit->text().toDouble();
    std::cout << "Set goalDu = " << solver->optimisations.adaptiveGoalDu << std::endl;
  });
  goalDuEdit->setDisabled(true);

  connect(adaptiveDtCheckBox, &QCheckBox::stateChanged, [=, this]() {
    solver->optimisations.adaptiveDt = adaptiveDtCheckBox->isChecked();
    goalDuEdit->setEnabled(solver->optimisations.adaptiveDt);
  });

  connect(differentiationBtn, &QPushButton::clicked, [=, this]() {
    solver->currentU = solver->currentU.derivative();
    plotCurrentU();
  });
  connect(stepBtn, &QPushButton::clicked, [=, this]() { step(); });
  connect(controlBtn, &QPushButton::clicked, [=, this]() {
    if (controlBtn->text() == "Start") {
      _timerId = startTimer(50);
      _start_step = _step;
      controlBtn->setText("Stop");
    } else {
      killTimer(_timerId);
      controlBtn->setText("Start");
    }
  });
  connect(reinitBtn, &QPushButton::clicked, [=, this]() {
    solver->totalTime = 0;
    setupExpression(getExpression());
  });
  connect(rescaleBtn, &QPushButton::clicked, [=, this]() { plotCurrentU(true); });
  connect(exportBtn, &QPushButton::clicked, [=, this]() {
    std::ofstream out_file("/tmp/heat-state.csv");
    Vector X = xt::linspace(-1.0, 1.0, N_LINSPACE_POINTS_TO_PLOT);
    xt::dump_csv(out_file, xt::atleast_2d(solver->currentU.evaluateOn(X)));
    std::cout << "Exported u(x) in its current state to /tmp/heat-state.csv" << std::endl;
  });

  auto mainWidget = new QWidget(this);
  auto mainLayout = new QGridLayout(mainWidget);
  auto sideLayout = new QVBoxLayout();
  sideLayout->addSpacing(5);
  sideLayout->addWidget(expressionLabel);
  sideLayout->addWidget(expressionLineEdit);
  sideLayout->addSpacing(5);
  sideLayout->addWidget(orderLabel);
  sideLayout->addWidget(orderEdit);
  sideLayout->addWidget(showChebpoints);
  sideLayout->addSpacing(5);
  sideLayout->addWidget(dtLabel);
  sideLayout->addWidget(dtEdit);
  sideLayout->addSpacing(5);
  sideLayout->addWidget(adaptiveDtCheckBox);
  sideLayout->addWidget(goalDuLabel);
  sideLayout->addWidget(goalDuEdit);
  sideLayout->addSpacing(5);
  // sideLayout->addWidget(linearMultistepCheckBox);
  sideLayout->addWidget(controlBtn);
  sideLayout->addWidget(stepBtn);
  sideLayout->addWidget(differentiationBtn);
  sideLayout->addWidget(reinitBtn);
  sideLayout->addWidget(rescaleBtn);
  sideLayout->addWidget(exportBtn);
  sideLayout->addWidget(createThemeChooser());
  sideLayout->addSpacing(5);
  sideLayout->addWidget(statsLabel);
  sideLayout->addStretch();
  mainLayout->addWidget(temperatureView, 0, 0);
  mainLayout->addLayout(sideLayout, 0, 1);
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
