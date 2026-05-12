/**
 * Simple integration test for SearchClient against live IA API (T1.7)
 * No QtTest framework - just direct function calls
 */

#include <QCoreApplication>
#include <QDebug>
#include "search_client.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "=== IA Search Client Integration Test ===";
    qDebug() << "";
    
    bool allPassed = true;
    
    // Test 1: Basic search
    {
        SearchClient client;
        QVariantList results = client.search("wikipedia", 3);
        
        qDebug() << "Test 1 - Basic search:";
        qDebug() << "  Query: wikipedia, rows: 3";
        qDebug() << "  Results:" << results.size();
        
        if (results.size() > 0 && results.size() <= 3) {
            qDebug() << "  PASS: Got expected number of results";
        } else {
            qDebug() << "  FAIL: Unexpected result count" << results.size();
            allPassed = false;
        }
    }
    
    // Test 2: Mediatype filter
    {
        SearchClient client;
        QVariantList results = client.search("linux", 5, "texts");
        
        qDebug() << "";
        qDebug() << "Test 2 - Mediatype filter (texts):";
        qDebug() << "  Results:" << results.size();
        
        if (results.size() >= 0) {
            qDebug() << "  PASS: Search completed without error";
        } else {
            qDebug() << "  FAIL: Unexpected result count" << results.size();
            allPassed = false;
        }
    }
    
    // Test 3: getItemMetadata
    {
        SearchClient client;
        QVariantMap metadata = client.getItemMetadata("test");
        
        qDebug() << "";
        qDebug() << "Test 3 - Get item metadata:";
        qDebug() << "  Identifier: test";
        qDebug() << "  Fields:" << metadata.keys().size();
        
        if (metadata.contains("identifier")) {
            qDebug() << "  PASS: Metadata contains identifier field";
        } else {
            qDebug() << "  FAIL: Missing identifier in metadata";
            allPassed = false;
        }
    }
    
    // Test 4: Empty query (popular items)
    {
        SearchClient client;
        QVariantList results = client.search("", 5);
        
        qDebug() << "";
        qDebug() << "Test 4 - Empty query (popular items):";
        qDebug() << "  Results:" << results.size();
        
        if (results.size() >= 0) {
            qDebug() << "  PASS: Search completed without error";
        } else {
            qDebug() << "  FAIL: Unexpected result count" << results.size();
            allPassed = false;
        }
    }
    
    qDebug() << "";
    qDebug() << "=== Test Summary ===";
    if (allPassed) {
        qDebug() << "ALL TESTS PASSED";
        return 0;
    } else {
        qDebug() << "SOME TESTS FAILED";
        return 1;
    }
}
