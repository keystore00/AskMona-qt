#include "units.h"
namespace
{
  enum Unit {mega, kilo, none, milli, micro, satoshi};
  QString name(Unit unit)
  {
    switch (unit) {
    case mega: return QString("M");
    case kilo: return QString("k");
    case none: return QString("");
    case milli: return QString("m");
    case micro: return QString("μ");
    case satoshi: return QString("0n");
    default: return QString("");
    }
  }
  qint64 factor(Unit unit)
  {
    qint64 base = 100000000;
    switch (unit) {
    case mega: return 1000000*base;
    case kilo: return 1000*base;
    case none: return 1*base;
    case milli: return base/1000;
    case micro: return base/1000000;
    case satoshi: return base/100000000;
    default: return 1*base;
    }
  }
  Unit decimals2unit(int decimals)
  {
    auto d = div(decimals, 3);
    auto quotinent = d.quot + (d.rem < 0 ? -1 : 0);
    switch (quotinent) {
    case 2: return mega;
    case 1: return kilo;
    case 0: return none;
    case -1: return milli;
    case -2: return micro;
    case -3: return satoshi;
    default: return none;
    }
  }
  int count_decimals(qint64 value)
  {
    int n_decimals =  -8;
    while (value >= 10) {
      value /= 10;
      ++n_decimals;
    }
    return n_decimals;
  }
}  // namespace
QString toHumanReadable(qint64 satoshi)
{
  if (satoshi == 0) {
    return QString("0");
  }
  auto decimals = count_decimals(satoshi);
  auto unit = decimals2unit(decimals);
  return QString::number(satoshi/(factor(unit))) + name(unit);
}
