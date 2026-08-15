#include <QtTest>

class Checks : public QObject {
    Q_OBJECT

private slots:
    void test_math() {
        QCOMPARE(2 + 2, 4);
    }
};

QTEST_MAIN(Checks)

#include "Checks.moc"

/**
 * initTestCase()
 * cleanupTestCase()
 */
