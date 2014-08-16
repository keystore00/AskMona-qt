#ifndef TOPICOBJECT_H
#define TOPICOBJECT_H
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <memory>
class TopicObject;
typedef std::shared_ptr<const TopicObject> TopicObjectPtr;
class TopicObject
{
public:
  static TopicObjectPtr New(const QJsonObject& json);
  ~TopicObject() = default;
  int rank,t_id,cat_id,count,favorites;
  unsigned long long receive;
  unsigned long long  created,updated,modified;
  QString title,category,lead,tags;
  QJsonObject obj;
  int category_flag;
  static int cat_id2flag(int cat_id);
private:
  TopicObject(const QJsonObject& json);
  TopicObject() = delete;
  TopicObject(const TopicObject &) = delete;
  TopicObject(TopicObject &&) = delete;
  TopicObject &operator=(const TopicObject&) = delete;
  TopicObject &operator=(TopicObject&&) = delete;
};

#endif // TOPICOBJECT_H
