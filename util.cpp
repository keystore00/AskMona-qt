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
QString getValidFileName(const QString& filename)
{
  QFile file(getDataDir()+"/"+filename);
  if (!file.exists()) {
    return ":/"+filename;
  } else {
    return file.fileName();
  }
}
QByteArray loadEmbeddedOrFile(const QString& filename)
{
  auto valid_name = getValidFileName(filename);
  QFile file(valid_name);
  if (!valid_name.startsWith(":")) {
    qDebug() << "The file is loaded from" << file.fileName();
  }
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  return file.readAll();
}
