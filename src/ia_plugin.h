#ifndef IA_PLUGIN_H
#define IA_PLUGIN_H

#include <QObject>
#include <QString>
#include "ia_interface.h"
#include "logos_api.h"
#include "logos_sdk.h"

/**
 * @brief Logos IA module plugin implementation
 */
class IaPlugin : public QObject, public IaInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IaInterface_iid FILE "metadata.json")
    Q_INTERFACES(IaInterface PluginInterface)

public:
    explicit IaPlugin(QObject* parent = nullptr);
    ~IaPlugin() override;

    // PluginInterface implementation
    QString name() const override { return "logos_ia"; }
    QString version() const override { return "0.1.0"; }

    // IaInterface implementation
    Q_INVOKABLE QVariantList search(const QString& query, int rows = 20) override;
    Q_INVOKABLE QVariantMap getItemMetadata(const QString& identifier) override;

    // LogosAPI initialization
    Q_INVOKABLE void initLogos(LogosAPI* logosAPIInstance);

signals:
    void searchResultsReady(const QVariantList& results);
    void eventResponse(const QString& eventName, const QVariantList& args);

private:
    LogosModules* logos = nullptr;
    LogosAPI* logosAPI = nullptr;
};

#endif // IA_PLUGIN_H
