#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QWebView>
class QTimeLine;
class MyWebView : public QWebView
{
  Q_OBJECT
public:
  using QWebView::load;
  explicit MyWebView(QWidget *parent = 0);
protected:
  void mousePressEvent(QMouseEvent* e) override;
  void mouseMoveEvent(QMouseEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void wheelEvent (QWheelEvent *e) override;
signals:
  void addNG(const QString& t_id);
  void zoomFactorChanged(qreal);

public slots:
  void scrollToTop();
  void scrollToBottom();
  void customMenuRequested(QPoint pos);
  void handleMouseGesture(const QString&);
  void setScrollBarPos(int pos);
  void load(const QString& url);
private:
  QTimeLine* timeLine;
};

#endif // MYWEBVIEW_H
