#ifndef MYWEBPAGE_H
#define MYWEBPAGE_H

#include <QWebPage>

class MyWebPage : public QWebPage
{
  Q_OBJECT
public:
  explicit MyWebPage(QObject *parent = 0);

signals:

public slots:
};

#endif  // MYWEBPAGE_H
