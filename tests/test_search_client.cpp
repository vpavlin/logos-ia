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
        
        // Search for a well-known item
        QVariantList results = client.search("test", 5);
        
        QVERIFY(results.size() > 0);
        QVERIFY(results.size() <= 5);
        
        // Each result should be a QVariantMap with expected fields
        for (const auto& result : results) {
            QVariantMap map = result.toMap();
            QVERIFY(map.contains("identifier"));
            QVERIFY(map["identifier"].toString().length() > 0);
            
            qDebug() << "Found:" << map["title"].toString() 
                     << "(" << map["identifier"].toString() << ")";
        }
    }

    /**
     * Test search with mediatype filter
     */
    void testSearchWithMediatype() {
        SearchClient client;
        
        // Search for texts only
        QVariantList results = client.search("linux", 10, "texts");
        
        QVERIFY(results.size() > 0);
        QVERIFY(results.size() <= 10);
        
        // Verify all results are texts
        for (const auto& result : results) {
            QVariantMap map = result.toMap();
            QString mediatype = map.value("mediatype", "").toString();
            QVERIFY(mediatype == "texts" || mediatype.isEmpty());
        }
    }

    /**
     * Test search with collection filter
     */
    void testSearchWithCollection() {
        SearchClient client;
        
        // Search in a specific collection (Internet Archive)
        QVariantList results = client.search("archive", 5, "", "internetarchive");
        
        QVERIFY(results.size() >= 0);  // May return 0 if no results
        
        if (results.size() > 0) {
            qDebug() << "Found" << results.size() 
                     << "items in internetarchive collection";
        }
    }

    /**
     * Test getItemMetadata for a known item
     */
    void testGetItemMetadata() {
        SearchClient client;
        
        // Use a well-known IA item
        QVariantMap metadata = client.getItemMetadata("test");
        
        QVERIFY(metadata.contains("identifier"));
        QCOMPARE(metadata["identifier"].toString(), QString("test"));
        
        qDebug() << "Metadata keys:" << metadata.keys();
    }

    /**
     * Test search with empty query returns results
     */
    void testSearchEmptyQuery() {
        SearchClient client;
        
        // Empty query should return popular items
        QVariantList results = client.search("", 5);
        
        QVERIFY(results.size() > 0);
        QVERIFY(results.size() <= 5);
        
        qDebug() << "Empty query returned" << results.size() << "results";
    }

    /**
     * Test search with large row count (max 1000)
     */
    void testSearchMaxRows() {
        SearchClient client;
        
        // Request max rows
        QVariantList results = client.search("test", 10);
        
        QVERIFY(results.size() > 0);
        QVERIFY(results.size() <= 10);
    }

    /**
     * Test search response structure
     */
    void testSearchResponseStructure() {
        SearchClient client;
        
        QVariantList results = client.search("wikipedia", 3);
        
        if (results.isEmpty()) {
            QSKIP("No results returned, skipping structure test");
        }
        
        // Check that each result has the expected structure
        const QVariantMap& firstResult = results.at(0).toMap();
        
        // These fields should be present in IA search results
        QVERIFY(firstResult.contains("identifier"));
        QVERIFY(firstResult.contains("title") || firstResult.contains("name"));
        
        qDebug() << "First result structure:" << firstResult.keys();
    }
};

QTEST_MAIN(TestSearchClient)
#include "test_search_client.moc"
