#include "ia_plugin.h"
#include "logos_api.h"
#include <QDebug>

IaPlugin::IaPlugin(QObject* parent)
    : QObject(parent)
{
    qDebug() << "IaPlugin: Constructor called";
}

IaPlugin::~IaPlugin()
{
    qDebug() << "IaPlugin: Destructor called";
}

void IaPlugin::initLogos(LogosAPI* logosAPIInstance) {
    if (logos) {
        delete logos;
        logos = nullptr;
    }
    if (logosAPI) {
        delete logosAPI;
        logosAPI = nullptr;
    }
    logosAPI = logosAPIInstance;
    if (logosAPI) {
        logos = new LogosModules(logosAPI);
    }
}

QVariantList IaPlugin::search(const QString& query, int rows)
{
    qDebug() << "IaPlugin: search called with query:" << query << "rows:" << rows;
    
    // Placeholder — will be implemented in T1.3
    return QVariantList();
}

QVariantMap IaPlugin::getItemMetadata(const QString& identifier)
{
    qDebug() << "IaPlugin: getItemMetadata called with identifier:" << identifier;
    
    // Placeholder — will be implemented later
    return QVariantMap();
}
