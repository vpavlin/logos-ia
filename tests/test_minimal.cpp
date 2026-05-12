#include <QCoreApplication>
#include <QDebug>
#include "search_client.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "Starting...";
    
    {
        SearchClient client;
        qDebug() << "SearchClient created";
        
        auto results = client.search("test", 1);
        qDebug() << "Search returned" << results.size() << "results";
        
        qDebug() << "About to destroy SearchClient...";
    }
    
    qDebug() << "SearchClient destroyed safely";
    qDebug() << "Done!";
    return 0;
}
