#ifndef TOPICLIST_H
#define TOPICLIST_H

#include <QWidget>
#include <map>
#include "topicobject.h"
class QLabel;
class QTextBrowser;
class QNetworkAccessManager;
class QNetworkReply;
class QComboBox;
class QSpinBox;
class QPushButton;
class TopicView;
class TopicList : public QWidget
{
  Q_OBJECT
public:
  explicit TopicList(QWidget *parent = 0);
  ~TopicList();
  typedef std::pair<TopicObjectPtr,TopicObjectPtr> TopicPair;
  typedef std::map<int,TopicPair> TopicPairListType;

signals:
  void topicClicked(const QString& t_id);
public slots:
  void addTopic(const QString& t_id);
  void update_view();
  void linkClicked(const QUrl&);
  void update();
  void handleMouseGesture(const QString&);
  const TopicObjectPtr getTopic(const QString& t_id);
  const TopicObjectPtr getTopic(int t_id);
  void mark_all_as_read();
private:
  TopicView *text_area;
  QNetworkAccessManager *nam;
  QComboBox *cbox,*cat_box;
  QSpinBox *num_update_box;
  QPushButton *reload_button;
  TopicPairListType topics;
  void setWidgets();
  void loadTopics(const QString& filename);
  void saveTopics(const QString& filename);
private slots:
  void finishedSlot(QNetworkReply*);
  void comboBoxChanged(int);
};

#endif // TOPICLIST_H
