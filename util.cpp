#include "util.h"
#include <QFile>
#include <QCoreApplication>
#include <QDebug>
QString loadEmbeddedOrFile(const QString& filename)
{
  QFile file(QCoreApplication::applicationDirPath()+"/"+filename);
  if (!file.exists()) {
    file.setFileName(":/"+filename);
  } else {
    qDebug() << "File is loaded from" << file.fileName();
  }
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  return file.readAll();
}
