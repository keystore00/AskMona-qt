#include "mywebview.h"
#include <QMouseEvent>
#include <QDir>
#include <QMenu>
#include <QWebFrame>
#include <QDebug>
#include <QTimeLine>
#include <QWebElement>
#include <QDesktopServices>
#include "constants.h"

MyWebView::MyWebView(QWidget *parent) :
  QWebView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  timeLine = new QTimeLine(200, this);
}

void MyWebView::mousePressEvent(QMouseEvent* e)
{
  switch (e->button()) {
  case Qt::RightButton:
    e->ignore();
    break;
  default:
    QWebView::mousePressEvent(e);
    e->ignore();
    break;
  }
};
void MyWebView::mouseReleaseEvent(QMouseEvent* e)
{
  QWebView::mouseMoveEvent(e);
  e->ignore();
};
void MyWebView::mouseMoveEvent(QMouseEvent* e)
{
  QWebView::mouseMoveEvent(e);
  e->ignore();
};
void MyWebView::wheelEvent(QWheelEvent *e)
{
  if (e->modifiers() == Qt::ControlModifier){
    setZoomFactor(zoomFactor()+e->delta()/120/10.0);
    emit zoomFactorChanged(zoomFactor());
  } else {
    QWebView::wheelEvent(e);
  }
};

void MyWebView::scrollToTop() {
  auto cur_pos = static_cast<double>(this->page()->mainFrame()->scrollBarValue(Qt::Vertical))/this->page()->mainFrame()->contentsSize().height() * 100;
  timeLine->setFrameRange(cur_pos, 0);
  connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setScrollBarPos(int)));
  timeLine->start();
}
void MyWebView::scrollToBottom() {
  auto cur_pos = static_cast<double>(this->page()->mainFrame()->scrollBarValue(Qt::Vertical))/this->page()->mainFrame()->contentsSize().height() * 100;
  timeLine->setFrameRange(cur_pos, 100);
  connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(setScrollBarPos(int)));
  timeLine->start();
}
void MyWebView::setScrollBarPos(int pos)
{
  this->page()->mainFrame()->setScrollBarValue(Qt::Vertical, this->page()->mainFrame()->contentsSize().height()*pos/100);
}

void MyWebView::customMenuRequested(QPoint pos){
  QMenu *menu = new QMenu(this);
  menu->addAction(pageAction(QWebPage::Reload));
  auto link_url = page()->mainFrame()->hitTestContent(pos).linkElement().attribute("href");
  if (url().toString() == ask_url_base+"/" && link_url.contains(QRegExp("^/\\d+$"))) {
    auto qa = new QAction("NG", menu);
    menu->addAction(qa);
    auto t_id = link_url.mid(1);
    connect(qa, &QAction::triggered, [this, t_id](){addNG(t_id);});
  }
  if (!link_url.isEmpty()) {
    auto qa = new QAction("Open in default application", menu);
    menu->addAction(qa);
    connect(qa, &QAction::triggered, [this, link_url](){QDesktopServices::openUrl(QUrl(link_url));});
  }
  if (!selectedText().isEmpty()) {
    auto url = "https://www.google.co.jp/search?q="+QString(QUrl::toPercentEncoding(selectedText()));
    auto qa = new QAction("Search Google for \""+(selectedText().length() > 5 ? selectedText().mid(0, 5)+"..." : selectedText())+"\"", menu);
    menu->addAction(qa);
    connect(qa, &QAction::triggered, [this, url](){load(url);});
  }
  if (!selectedText().isEmpty()) {
    menu->addAction(pageAction(QWebPage::Copy));
  }
  menu->popup(mapToGlobal(pos));
}

void MyWebView::handleMouseGesture(const QString& g)
{
  if (!underMouse()) {
    return;
  }
  if (g == "LD") {
    scrollToBottom();
    return;
  } else if (g == "LU") {
    scrollToTop();
    return;
  } else if (g == "UD") {
    triggerPageAction(QWebPage::Reload);
    return;
  } else if (g == "") {
    customMenuRequested(mapFromGlobal(QCursor::pos()));
  }
}
void MyWebView::load(const QString& url)
{
  load(QUrl(url));
}
