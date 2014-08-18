#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QNetworkCookieJar>

class CookieJar : public QNetworkCookieJar
{
  Q_OBJECT
public:
  explicit CookieJar(QObject *parent = 0);
  ~CookieJar();

signals:

public slots:
private:
  void loadCookies(const QString& filename);
  void saveCookies(const QString& filename);
  void purgeOldCookies();
};

#endif  // COOKIEJAR_H
