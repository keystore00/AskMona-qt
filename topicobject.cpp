#include "topicobject.h"
#include <QJsonArray>
#include <QVariant>
#include <algorithm>

TopicObject::TopicObject(const QJsonObject& json)
  : title(json["title"].toString()), category(json["category"].toString()), lead(json["lead"].toString()), tags(json["tags"].toString()), obj(json)
{
  rank = json["rank"].toInt();
  t_id = json["t_id"].toInt();
  cat_id = json["cat_id"].toInt();
  created = json["created"].toVariant().toULongLong();
  updated = json["updated"].toVariant().toULongLong();
  modified = json["modified"].toVariant().toULongLong();
  count = json["count"].toInt();
  receive = json["receive"].toString().toULongLong();
}

TopicObjectPtr TopicObject::New(const QJsonObject& json)
{
  return TopicObjectPtr(new TopicObject(json));
}
