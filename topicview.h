#ifndef TOPICVIEW_H
#define TOPICVIEW_H

#include <QTextBrowser>

class TopicView : public QTextBrowser
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
};

template <typename TopicPairList>
void TopicView::set(const TopicPairList& topics)
{
  static const QString topic_view_style =
    "<style type=text/css>"
    "* {background-color:white}"
    "a:link {color:black; text-decoration:none;}"
    "a:hover {color:blue; text-decoration:underline}"
    "a.del:link {color:red; text-decoration:none;}"
    "a.del:hover {color:red; text-decoration:underline}"
    ".res_updated {color:blue;}"
    ".mona_updated {color:green;}"
    "</style>";
  QString html = "<html>" + topic_view_style;
  for (const auto &t : topics) {
    auto &last = t.second->first;
    auto &current = t.second->second;
    auto t_id = QString::number(last["t_id"].toInt());
    html+="<div>";
    html+="<a class=\"del\" href=\"x_"+t_id+"\"> X </a>";
    html+="<a class=\"t_link\" href=\"t_"+t_id+"\">";
    html+=last["title"].toString();
    auto dr = current["count"].toInt() - last["count"].toInt();
    if (dr>0) {
      html+="<span class=\"res_updated\"> res +" + QString::number(dr)+"</span>";
    } else {
      html+="<span> res +" + QString::number(dr)+"</span>";
    }
    auto dm = (current["receive"].toString().toULongLong() - last["receive"].toString().toULongLong())/100000000.0;
    if (dm>0) {
      html+="<span class=\"mona_updated\"> MONA +" + QString::number(dm)+"</span>";
    } else {
      html+="<span> MONA +" + QString::number(dm)+"</span>";
    }
    html+="</a>";
    html+="</div>";
  }
  html += "</html>";
  setText(html);
}

#endif // TOPICVIEW_H
