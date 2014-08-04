#ifndef TOPICVIEW_H
#define TOPICVIEW_H

#include <QWebView>
#include <QWebFrame>

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
  //%1 topic id
  //%2 topic title
  //%3 res. count class [updated,noupdate]
  //%4 res. count increased
  //%5 mona class [updated,noupdate]
  //%6 mona increased
  QString body;
  for (const auto &t : topics) {
    auto &last = t.second->first;
    auto &current = t.second->second;
    auto t_id = QString::number(last["t_id"].toInt());
    auto dr = current["count"].toInt() - last["count"].toInt();
    QString res_class = dr?"updated":"noupdate";
    auto dm = (current["receive"].toString().toULongLong() - last["receive"].toString().toULongLong())/100000000.0;
    QString mona_class = dm?"updated":"noupdate";
    body += topic_view_item.arg(t_id).arg(last["title"].toString()).arg(res_class).arg(QString::number(dr)).arg(mona_class).arg(QString::number(dm));
  }
  auto tmp = topic_view_html.arg(topic_view_style).arg(body);
  //qDebug("%s",qPrintable(tmp));
  page()->mainFrame()->setHtml(tmp);
}

#endif // TOPICVIEW_H
