#ifndef UTIL_H
#define UTIL_H
#include <QString>
QByteArray loadEmbeddedOrFile(const QString& filename);
void setDataDir(const QString& dir);
QString getDataDir();
/// Return filename if "datadir/filename" exists, else return ":/filename"(embedded resource)
QString getValidFileName(const QString& filename);
#endif // UTIL_H
