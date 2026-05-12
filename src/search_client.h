#ifndef SEARCH_CLIENT_H
#define SEARCH_CLIENT_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

/**
 * @brief Internet Archive Search API client
 * 
 * Handles HTTP requests to archive.org/advancedsearch.php and parses
 * JSON responses into structured QVariantList results.
 */
class SearchClient : public QObject
{
    Q_OBJECT

public:
    explicit SearchClient(QObject* parent = nullptr);
    ~SearchClient() override;


    /**
     * @brief Search Internet Archive
     * @param query Search query string
     * @param rows Number of results to return (max 1000)
     * @param mediatype Optional mediatype filter (e.g. "texts", "audio", "image")
     * @param collections Optional comma-separated list of collection IDs to filter by
     * @return QVariantList of search results
     */
    QVariantList search(const QString& query, int rows = 20,
                        const QString& mediatype = QString(),
                        const QString& collections = QString());

    /**
     * @brief Get item metadata from Internet Archive
     * @param identifier IA item identifier
     * @return QVariantMap with metadata, or empty map on error
     */
    QVariantMap getItemMetadata(const QString& identifier);

signals:
    /**
     * @brief Emitted when search completes successfully
     * @param results The search results
     */
    void searchCompleted(const QVariantList& results);

    /**
     * @brief Emitted when an error occurs during a network request
     * @param errorString Description of the error
     */
    void errorOccurred(const QString& errorString);

private slots:
    void onSearchReplyFinished();
    void onMetadataReplyFinished();

private:
    /**
     * @brief Build IA search API URL from parameters
     */
    QUrl buildSearchUrl(const QString& query, int rows,
                        const QString& mediatype,
                        const QString& collections) const;

    /**
     * @brief Parse IA search response JSON into QVariantList
     */
    QVariantList parseSearchResponse(const QByteArray& data);

    /**
     * @brief Parse IA metadata response JSON into QVariantMap
     */
    QVariantMap parseMetadataResponse(const QByteArray& data);

    /**
     * @brief Get a standardized user-agent string for IA requests
     */
    static QString userAgent();

    QNetworkAccessManager m_networkManager;
    QNetworkReply* m_currentReply = nullptr;
};

#endif // SEARCH_CLIENT_H
