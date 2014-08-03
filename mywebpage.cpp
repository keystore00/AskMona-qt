#include "mywebpage.h"
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QCoreApplication>
#include "cookiejar.h"

MyWebPage::MyWebPage(QObject *parent) :
  QWebPage(parent)
{
  auto pNAM = networkAccessManager();
  QNetworkDiskCache * pNDC = new QNetworkDiskCache(parent);
  pNDC->setCacheDirectory( QCoreApplication::applicationDirPath()+"/cache" );

  qint64 size = pNDC->cacheSize();
  size = pNDC->maximumCacheSize();
  const qint64 desired = 1024*1024*1024;

  if (size < desired) {
    pNDC->setMaximumCacheSize(desired);
    size = pNDC->maximumCacheSize();
  }

  QString dir = pNDC->cacheDirectory();
  pNAM->setCache(pNDC);

  auto cj = new CookieJar(parent);
  pNAM->setCookieJar(cj);
}
