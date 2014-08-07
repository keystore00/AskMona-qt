#include "topicview.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QDebug>
#include "util.h"

TopicView::TopicView(QWidget *parent) :
    QWebView(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  topic_view_html = loadEmbeddedOrFile("topiclist.html");
  topic_view_item = loadEmbeddedOrFile("topiclist_item.html");
  topic_view_style = loadEmbeddedOrFile("topiclist.css");
}

void TopicView::mousePressEvent(QMouseEvent* e)
{
  switch (e->button()) {
  case Qt::RightButton:
    e->ignore();
    break;
  default:
    QWebView::mousePressEvent(e);
    break;
  }
};
void TopicView::mouseReleaseEvent(QMouseEvent* e)
{
  QWebView::mouseMoveEvent(e);
  e->ignore();
};
void TopicView::mouseMoveEvent(QMouseEvent* e)
{
  QWebView::mouseMoveEvent(e);
  e->ignore();
};
