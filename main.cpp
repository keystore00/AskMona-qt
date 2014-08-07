#include "mainwindow.h"
#include <QApplication>
#include "util.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  a.setStyleSheet(loadEmbeddedOrFile("app_style.css"));

  return a.exec();
}
