#include "topicobject.h"
#include <QJsonArray>
#include <QVariant>

TopicObject::TopicObject(const QJsonObject& json)
  : title(json["title"].toString()),
    category(json["category"].toString()),
    lead(json["lead"].toString()),
    tags(json["tags"].toString()),
    obj(json)
{
  rank = json["rank"].toInt();
  t_id = json["t_id"].toInt();
  cat_id = json["cat_id"].toInt();
  category_flag = cat_id2flag(cat_id);
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

int TopicObject::cat_id2flag(int cat_id)
{
  return 1 << cat_id;
}
