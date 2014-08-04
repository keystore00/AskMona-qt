#ifndef TOPICLIST_H
#define TOPICLIST_H

#include <QWidget>
#include <QJsonObject>
#include <map>
class QLabel;
class QTextBrowser;
class QNetworkAccessManager;
class QNetworkReply;
class QComboBox;
class QSpinBox;
class TopicView;
class TopicList : public QWidget
{
  Q_OBJECT
public:
  explicit TopicList(QWidget *parent = 0);
  ~TopicList();
  typedef std::pair<QJsonObject,QJsonObject> TopicPair;
  typedef std::map<int,TopicPair> TopicPairListType;

signals:
  void topicClicked(const QString& t_id);
public slots:
  void addTopic(const QString& t_id);
  void update_view();
  void linkClicked(const QUrl&);
  void update();
  void handleMouseGesture(const QString&);
  const QJsonObject& getTopic(const QString& t_id);
  const QJsonObject& getTopic(int t_id);
  void mark_all_as_read();
private:
  TopicView *text_area;
  QNetworkAccessManager *nam;
  QComboBox *cbox;
  QSpinBox *num_update_box;
  TopicPairListType topics;
  void setWidgets();
  void loadTopics(const QString& filename);
  void saveTopics(const QString& filename);
private slots:
  void finishedSlot(QNetworkReply*);
  void comboBoxChanged(int);
};

#endif // TOPICLIST_H
