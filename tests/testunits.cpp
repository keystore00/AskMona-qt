#include <QtTest/QtTest>
#include "units.h"
class TestUnits: public QObject
{
  Q_OBJECT
private slots:
  void toHumanReadable_data();
  void toHumanReadable();
};
void TestUnits::toHumanReadable_data()
{
  QTest::addColumn<qint64>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("0") << (qint64)0 << "0";
  QTest::newRow("1") << (qint64)1 << "10n";
  QTest::newRow("23") << (qint64)23 << "230n";
  QTest::newRow("456") << (qint64)456 << "4μ";
  QTest::newRow("7891") << (qint64)7891 << "78μ";
  QTest::newRow("23456") << (qint64)23456 << "234μ";
  QTest::newRow("789123") << (qint64)789123 << "7m";
  QTest::newRow("4567891") << (qint64)4567891 << "45m";
  QTest::newRow("23456789") << (qint64)23456789 << "234m";
  QTest::newRow("123456789") << (qint64)123456789 << "1";
  QTest::newRow("1234567891") << (qint64)1234567891 << "12";
  QTest::newRow("23456789123") << (qint64)23456789123 << "234";
  QTest::newRow("456789123456") << (qint64)456789123456 << "4k";
  QTest::newRow("7891234567891") << (qint64)7891234567891 << "78k";
  QTest::newRow("23456789123456") << (qint64)23456789123456 << "234k";
  QTest::newRow("789123456789123") << (qint64)789123456789123 << "7M";
  QTest::newRow("4567891234567891") << (qint64)4567891234567891 << "45M";
  QTest::newRow("23456789123456789") << (qint64)23456789123456789 << "234M";
}

void TestUnits::toHumanReadable()
{
  QFETCH(qint64, value);
  QFETCH(QString, result);

  QCOMPARE(::toHumanReadable(value), result);
}

QTEST_MAIN(TestUnits)
#include "testunits.moc"
