#include "resobject.h"
#include <QVariant>

ResObject::ResObject(const QJsonObject& json)
  : u_dan(json["u_dan"].toString()), u_times(json["u_times"].toString()), receive(json["receive"].toString()), response(json["response"].toString()), obj(json)
{
  r_id = json["r_id"].toInt();
  u_id = json["u_id"].toInt();
  res_lv = json["res_lv"].toInt();
  res_count = json["res_count"].toInt();
  created = json["created"].toVariant().toULongLong();
}

ResObjectPtr ResObject::New(const QJsonObject& json)
{
  return ResObjectPtr(new ResObject(json));
}
