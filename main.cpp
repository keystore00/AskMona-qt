#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "util.h"
using namespace std;

void parseCommandLine() {
  QCommandLineParser parser;
  parser.setApplicationDescription("An Ask Mona browser.");
  const auto helpOption = parser.addHelpOption();
  const auto versionOption = parser.addVersionOption();
  QCommandLineOption dataDirectoryOption(QStringList() << "d" << "datadir","All data are saved into <directory>. Default is application directory.","directory");
  parser.addOption(dataDirectoryOption);

  if (!parser.parse(QCoreApplication::arguments())) {
    cout << qPrintable(parser.errorText()) << endl;
    exit(1);
  }
  if (parser.isSet(versionOption)) {
    cout << qPrintable(QCoreApplication::applicationName()) << " " << qPrintable(QCoreApplication::applicationVersion()) << endl;
    exit(0);
  }
  if (parser.isSet(helpOption)) {
    parser.showHelp();
    exit(0);
  }
  if (parser.isSet(dataDirectoryOption)) {
    setDataDir(parser.value(dataDirectoryOption));
  }
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QCoreApplication::setApplicationName("AskMona-qt");
  QCoreApplication::setApplicationVersion("0.1");
  parseCommandLine();

  MainWindow w;
  w.show();
  a.setStyleSheet(loadEmbeddedOrFile("app_style.css"));

  return a.exec();
}
