#include "mywebview.h"
#include <QMouseEvent>
#include <QDir>
#include <QMenu>
#include <QWebFrame>
#include <QDebug>
#include <QTimeLine>
#include <QWebElement>
#include <QSignalMapper>
#include <iostream>
#include "constants.h"
using namespace std;

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
  QMenu *menu=new QMenu(this);
  menu->addAction(pageAction(QWebPage::Reload));
  auto link = page()->mainFrame()->hitTestContent(pos).linkElement();
  if (url().toString()==ask_url_base+"/" && !link.attribute("href").isEmpty()) {
    auto qa = new QAction("NG", menu);
    menu->addAction(qa);
    auto signalMapper = new QSignalMapper(menu);
    connect(qa,SIGNAL(triggered()),signalMapper,SLOT(map()));
    auto t_id = link.attribute("href").mid(1);
    signalMapper->setMapping(qa,t_id);
    connect (signalMapper, SIGNAL(mapped(QString)), this, SIGNAL(addNG(QString))) ;
  }
  if (!selectedText().isEmpty()) {
    auto url = "https://www.google.co.jp/search?q="+QString(QUrl::toPercentEncoding(selectedText()));
    auto qa = new QAction("Search Google for \""+(selectedText().length()>5?selectedText().mid(0,5)+"...":selectedText())+"\"", menu);
    menu->addAction(qa);
    auto signalMapper = new QSignalMapper(menu);
    connect(qa,SIGNAL(triggered()),signalMapper,SLOT(map()));
    signalMapper->setMapping(qa,url);
    connect (signalMapper, SIGNAL(mapped(QString)), this, SLOT(load(QString)));
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
  } if (g == "") {
    customMenuRequested(mapFromGlobal(QCursor::pos()));
  }
}
void MyWebView::load(const QString& url)
{
  qDebug() << url;
  load(QUrl(url));
}
