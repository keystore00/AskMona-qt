#ifndef RESOBJECT_H
#define RESOBJECT_H
#include <QString>
#include <QJsonObject>
#include <memory>
class ResObject;
typedef std::shared_ptr<const ResObject> ResObjectPtr;

class ResObject
{
public:
  static ResObjectPtr New(const QJsonObject& json);
  ~ResObject() = default;
  int r_id, u_id, res_lv, res_count;
  qint64 created;
  QString u_dan, u_times, receive, response;
  QJsonObject obj;
private:
  explicit ResObject(const QJsonObject& json);
  ResObject() = delete;
  ResObject(const ResObject &) = delete;
  ResObject(ResObject &&) = delete;
  ResObject &operator=(const ResObject) = delete;
  ResObject &operator=(ResObject&&) = delete;
};

#endif  // RESOBJECT_H
