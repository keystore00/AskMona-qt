#include "util.h"
#include <QFile>
#include <QCoreApplication>
#include <QDebug>
namespace {
  QString datadir = "";
}
void setDataDir(const QString& dir) {
  datadir = dir;
}
QString getDataDir()
{
  if (datadir.isEmpty()) {
    return QCoreApplication::applicationDirPath();
  } else {
    return datadir;
  }
}
QString loadEmbeddedOrFile(const QString& filename)
{
  QFile file(getDataDir()+"/"+filename);
  if (!file.exists()) {
    file.setFileName(":/"+filename);
  } else {
    qDebug() << "The file is loaded from" << file.fileName();
  }
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  return file.readAll();
}
