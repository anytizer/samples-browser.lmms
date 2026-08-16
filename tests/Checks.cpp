#include <QtTest>

class Checks : public QObject {
    Q_OBJECT

private slots:

    void initTestCase() {
        qDebug("Running setup before any tests...");
        // Initialize shared resources here
    }

    void cleanupTestCase() {
        qDebug("Running cleanup after all tests...");
    }

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
