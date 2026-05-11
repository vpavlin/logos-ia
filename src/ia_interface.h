#ifndef IA_INTERFACE_H
#define IA_INTERFACE_H

#include <QObject>
#include <QString>
#include "interface.h"

/**
 * @brief Interface for the Logos IA module plugin
 * 
 * This interface defines the Logos plugin lifecycle methods.
 * The IA API (search, getItemMetadata) is exposed via Qt Remote Objects
 * through the IaBackend .rep interface.
 */
class IaInterface : public PluginInterface
{
public:
    virtual ~IaInterface() = default;

signals:
    /**
     * @brief Emitted when an event occurs in this module
     * @param eventName Name of the event
     * @param args Event arguments
     */
    void eventResponse(const QString& eventName, const QVariantList& args);
};

#define IaInterface_iid "org.logos.IaInterface"
Q_DECLARE_INTERFACE(IaInterface, IaInterface_iid)

#endif // IA_INTERFACE_H
