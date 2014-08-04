#include "topicview.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QDebug>
namespace {
  QString loadEmbeddedOrFile(const QString& filename) {
    QFile file(QCoreApplication::applicationDirPath()+"/"+filename);
    if (!file.exists()) {
      file.setFileName(":/"+filename);
    } else {
      qDebug() << "File is loaded from" << file.fileName();
    }
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    return file.readAll();
  }
}
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
