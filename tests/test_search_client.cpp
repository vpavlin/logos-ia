/**
 * @brief Integration tests for SearchClient against live Internet Archive API
 * 
 * These tests verify that the SearchClient correctly communicates with
 * archive.org's search API and parses responses properly.
 */

#include <QtTest/QtTest>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "search_client.h"

class TestSearchClient : public QObject
{
    Q_OBJECT

private slots:
    /**
     * Test basic search query against live IA API
     */
    void testBasicSearch() {
        SearchClient client;
        
        // Search for a well-known item - should return results
        QVariantList results = client.search("wikipedia", 3);
        
        qDebug() << "Got" << results.size() << "results";
        QVERIFY(results.size() > 0);
        QVERIFY(results.size() <= 3);
    }
};

QTEST_MAIN(TestSearchClient)
#include "test_search_client.moc"
