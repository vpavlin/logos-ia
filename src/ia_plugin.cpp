#include "ia_plugin.h"
#include "logos_api.h"
#include <QDebug>

IaPlugin::IaPlugin(QObject* parent)
    : QObject(parent)
{
    qDebug() << "IaPlugin: Constructor called";
    
    // Create SearchClient as child of this plugin
    m_searchClient = new SearchClient(this);
    
    // Connect SearchClient signals to our slots
    connect(m_searchClient, &SearchClient::searchCompleted,
            this, &IaPlugin::onSearchCompleted);
    connect(m_searchClient, &SearchClient::errorOccurred,
            this, &IaPlugin::onErrorOccurred);
}

IaPlugin::~IaPlugin()
{
    qDebug() << "IaPlugin: Destructor called";
}

void IaPlugin::initLogos(LogosAPI* logosAPIInstance) {
    // Delete old API (its destructor will delete the LogosModules child)
    delete logosAPI;
    logosAPI = nullptr;
    logos = nullptr;

    if (logosAPIInstance) {
        logosAPI = logosAPIInstance;
        logos = new LogosModules(logosAPI);
    }
}

QVariantList IaPlugin::search(std::string query, int rows)
{
    qDebug() << "IaPlugin: search called with query:" << QString::fromStdString(query) << "rows:" << rows;
    
    // Use SearchClient to perform the actual search
    return m_searchClient->search(QString::fromStdString(query), rows);
}

QVariantMap IaPlugin::getItemMetadata(std::string identifier)
{
    qDebug() << "IaPlugin: getItemMetadata called with identifier:" << QString::fromStdString(identifier);
    
    // Use SearchClient to get metadata
    return m_searchClient->getItemMetadata(QString::fromStdString(identifier));
}

QObject* IaPlugin::createRemoteObjectSource(QObject* parent)
{
    // Create a simple proxy that delegates to this plugin's search/getItemMetadata
    // The actual implementation would use QRemoteObjectHost to publish
    // For now, return 'this' since we implement the same interface
    Q_UNUSED(parent);
    return this;
}

void IaPlugin::onSearchCompleted(const QVariantList& results)
{
    qDebug() << "IaPlugin: search completed with" << results.size() << "results";
    
    // Also emit the Logos event for broader integration
    emit eventResponse("searchCompleted", QVariantList() << results.size());
}

void IaPlugin::onErrorOccurred(const QString& errorString)
{
    qWarning() << "IaPlugin: search error:" << errorString;
    
    // Emit error event for broader integration
    emit eventResponse("searchError", QVariantList() << errorString);
}
