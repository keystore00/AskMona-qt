#include "topicobject.h"
#include <QJsonArray>
#include <QVariant>

TopicObject::TopicObject(const QJsonObject& json)
  : title(json["title"].toString()), category(json["category"].toString()), lead(json["lead"].toString()), obj(json)
{
  rank = json["rank"].toInt();
  t_id = json["t_id"].toInt();
  cat_id = json["cat_id"].toInt();
  created = json["created"].toVariant().toULongLong();
  updated = json["updated"].toVariant().toULongLong();
  modified = json["modified"].toVariant().toULongLong();
  count = json["count"].toInt();
  receive = json["receive"].toString().toULongLong();
  if (json.contains("tags")) {
    auto vt = json["tags"].toArray();
    for (const auto& t : vt) {
      tags << t.toString();
    }
  }
}

TopicObjectPtr TopicObject::New(const QJsonObject& json)
{
  return TopicObjectPtr(new TopicObject(json));
}
