#ifndef UTIL_H
#define UTIL_H
#include <QString>
QString loadEmbeddedOrFile(const QString& filename);
void setDataDir(const QString& dir);
QString getDataDir();
#endif // UTIL_H
