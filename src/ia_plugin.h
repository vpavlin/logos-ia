#ifndef IA_PLUGIN_H
#define IA_PLUGIN_H

#include <string>
#include <QObject>
#include <QString>
#include "ia_interface.h"
#include "search_client.h"
#include "logos_api.h"
#include "logos_sdk.h"

/**
 * @brief Logos IA module plugin implementation
 * 
 * Implements IaInterface (Logos plugin interface) for lifecycle management.
 * Uses SearchClient for Internet Archive API communication.
 * Exposes search/getItemMetadata via Qt Remote Objects through IaBackendSource.
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

    // Qt Remote Objects provider methods (called by repc-generated IaBackendSourceAPI)
    QVariantList search(std::string query, int rows);
    QVariantMap getItemMetadata(std::string identifier);

    /**
     * @brief Expose this plugin as a Qt Remote Objects source
     * @param parent Parent QObject for the host
     * @return Pointer to IaBackendSource that can be published
     */
    QObject* createRemoteObjectSource(QObject* parent = nullptr);

    // LogosAPI initialization
    Q_INVOKABLE void initLogos(LogosAPI* logosAPIInstance);

signals:
    void eventResponse(const QString& eventName, const QVariantList& args);

private slots:
    void onSearchCompleted(const QVariantList& results);
    void onErrorOccurred(const QString& errorString);

private:
    SearchClient* m_searchClient = nullptr;
    LogosModules* logos = nullptr;
    LogosAPI* logosAPI = nullptr;
};

#endif // IA_PLUGIN_H
