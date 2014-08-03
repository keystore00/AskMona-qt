#include "cookiejar.h"
#include <QNetworkCookie>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QFile>

namespace {
  QString fname = "/dat/cookie.json";
}
CookieJar::CookieJar(QObject *parent) :
  QNetworkCookieJar(parent)
{
  loadCookies(QCoreApplication::applicationDirPath()+fname);
}

CookieJar::~CookieJar()
{
  saveCookies(QCoreApplication::applicationDirPath()+fname);
}

void CookieJar::loadCookies(const QString& filename)
{
  QFile cookieFile(filename);
  if (!cookieFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't read cookie file.");
    return;
  }
  QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(cookieFile.readAll());
  if (cookies.count() == 0) {
    qWarning() << "Failed to load cookie";
  } else {
    setAllCookies(cookies);
  }
}
void CookieJar::saveCookies(const QString& filename)
{
  purgeOldCookies();
  QFile cookieFile(filename);
  if (!cookieFile.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't save cookie file.");
    return;
  }
  auto cookies = allCookies();
  for (const auto &c:cookies) {
    cookieFile.write(c.toRawForm());
    cookieFile.write("\n");
  }
}

void CookieJar::purgeOldCookies()
{
  auto cookies = allCookies();
  if (cookies.isEmpty())
    return;
  auto now = QDateTime::currentDateTime();
  for (int i = cookies.count() - 1; i >= 0; --i) {
    if (!cookies.at(i).isSessionCookie() && cookies.at(i).expirationDate() < now)
      cookies.removeAt(i);
  }
  setAllCookies(cookies);
}
