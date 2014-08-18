#ifndef TOPICVIEW_H
#define TOPICVIEW_H

#include <QWebView>
#include <QWebFrame>
#include "units.h"
namespace {
  inline double fromSatoshi(unsigned long long val) {
    return val / 100000000.0;
  }
}
class TopicView : public QWebView
{
  Q_OBJECT
public:
  explicit TopicView(QWidget *parent = 0);
  template <typename TopicPairList>
  void set(const TopicPairList& topics);
protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
signals:

public slots:
private:
  QString topic_view_style;
  QString topic_view_html;
  QString topic_view_item;
};

template <typename TopicPairList>
void TopicView::set(const TopicPairList& topics)
{
  // %1 topic id
  // %2 topic title
  // %3 category id
  // %4 category
  // %5 tags
  // %6 lead
  // %7 created
  // %8 updated
  // %9 modified
  // %10 res. count
  // %11 res. count class [updated,noupdate]
  // %12 res. count increased from last visit
  // %13 mona received
  // %14 mona class [updated,noupdate]
  // %15 mona increased from last visit
  // %16 favorites
  QString body;
  for (const auto &t : topics) {
    auto &last = t.second->first;
    auto &current = t.second->second;
    auto t_id = QString::number(last->t_id);
    auto dr = current->count - last->count;
    QString res_class = dr?"updated":"noupdate";
    auto dm = current->receive - last->receive;
    QString mona_class = dm?"updated":"noupdate";
    auto tmp = topic_view_item.arg
      (t_id, last->title,
       QString::number(last->cat_id),
       last->category,
       last->tags,
       last->lead,
       QString::number(last->created),
       QString::number(last->updated),
       QString::number(last->modified));
    body += tmp.arg
      (QString::number(last->count),
       res_class,
       QString::number(dr),
       QString::number(fromSatoshi(last->receive)),
       mona_class, toHumanReadable(dm),
       QString::number(last->favorites));
  }
  auto tmp = topic_view_html.arg(topic_view_style, body);
  page()->mainFrame()->setHtml(tmp);
}

#endif  // TOPICVIEW_H
