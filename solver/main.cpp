#include "Interface.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  setlocale(LC_NUMERIC, "en_US.UTF-8");
  srand(time(NULL));

  HeatDemonstrator window;
  window.buildUI();
  window.resize(1380, 892);
  window.show();
  return app.exec();
}
