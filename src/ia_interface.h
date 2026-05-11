#ifndef IA_INTERFACE_H
#define IA_INTERFACE_H

#include <QObject>
#include <QString>
#include "interface.h"

/**
 * @brief Interface for the Logos IA module
 * 
 * This interface defines the public API that this module exposes
 * to other modules and applications.
 */
class IaInterface : public PluginInterface
{
public:
    virtual ~IaInterface() = default;
    
    /**
     * @brief Search Internet Archive
     * @param query Search query string
     * @param rows Number of results to return (max 1000)
     * @return List of search result items as QVariantList
     */
    Q_INVOKABLE virtual QVariantList search(const QString& query, int rows = 20) = 0;
    
    /**
     * @brief Get item metadata from Internet Archive
     * @param identifier IA item identifier
     * @return Item metadata as QVariantMap
     */
    Q_INVOKABLE virtual QVariantMap getItemMetadata(const QString& identifier) = 0;

signals:
    void searchResultsReady(const QVariantList& results);
    void eventResponse(const QString& eventName, const QVariantList& args);
};

#define IaInterface_iid "org.logos.IaInterface"
Q_DECLARE_INTERFACE(IaInterface, IaInterface_iid)

#endif // IA_INTERFACE_H
