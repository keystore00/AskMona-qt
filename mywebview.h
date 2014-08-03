#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QWebView>
class QTimeLine;
class MyWebView : public QWebView
{
  Q_OBJECT
public:
  explicit MyWebView(QWidget *parent = 0);
protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
signals:

public slots:
  void scrollToTop();
  void scrollToBottom();
  void customMenuRequested(QPoint pos);
  void handleMouseGesture(const QString&);
  void setScrollBarPos(int pos);
private:
  QTimeLine* timeLine;
};

#endif // MYWEBVIEW_H
