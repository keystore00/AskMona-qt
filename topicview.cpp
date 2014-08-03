#include "topicview.h"
#include <QMouseEvent>

TopicView::TopicView(QWidget *parent) :
    QTextBrowser(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
}

void TopicView::mousePressEvent(QMouseEvent* e)
{
  switch (e->button()) {
  case Qt::RightButton:
    e->ignore();
    break;
  default:
    QTextBrowser::mousePressEvent(e);
    break;
  }
};
void TopicView::mouseReleaseEvent(QMouseEvent* e)
{
  QTextBrowser::mouseMoveEvent(e);
  e->ignore();
};
void TopicView::mouseMoveEvent(QMouseEvent* e)
{
  QTextBrowser::mouseMoveEvent(e);
  e->ignore();
};
