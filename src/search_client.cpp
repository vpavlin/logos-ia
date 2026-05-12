#include "search_client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QEventLoop>
#include <QDebug>

SearchClient::SearchClient(QObject* parent)
    : QObject(parent)
{
    // Set up network manager with timeout
    m_networkManager.setTransferTimeout(30000); // 30 second timeout
}

SearchClient::~SearchClient()
{
    // Cancel any pending network request to avoid use-after-free
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}

QString SearchClient::userAgent()
{
    return "logos_ia/0.1.0";
}

QUrl SearchClient::buildSearchUrl(const QString& query, int rows,
                                   const QString& mediatype,
                                   const QString& collections) const
{
    QUrl url("https://archive.org/advancedsearch.php");
    
    QUrlQuery queryParams;
    queryParams.addQueryItem("q", query);
    queryParams.addQueryItem("rows", QString::number(qMin(rows, 1000)));
    queryParams.addQueryItem("output", "json");
    
    if (!mediatype.isEmpty()) {
        queryParams.addQueryItem("mediatype[]", mediatype);
    }
    
    if (!collections.isEmpty()) {
        // IA API expects multiple collection[]= values
        QStringList colls = collections.split(',', Qt::SkipEmptyParts);
        for (const QString& coll : colls) {
            queryParams.addQueryItem("collection[]", coll.trimmed());
        }
    }
    
    url.setQuery(queryParams);
    return url;
}

QVariantList SearchClient::search(const QString& query, int rows,
                                   const QString& mediatype,
                                   const QString& collections)
{
    QUrl url = buildSearchUrl(query, rows, mediatype, collections);
    
    qDebug() << "IA Search:" << url.toString();
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent());
    request.setRawHeader("Accept", "application/json");
    
    m_currentReply = m_networkManager.get(request);
    
    // Wait for synchronous completion (block until reply finishes)
    QEventLoop loop;
    connect(m_currentReply, &QNetworkReply::finished, &loop, &QEventLoop::quit, Qt::UniqueConnection);
    loop.exec();
    
    // Check if error occurred during the request
    // Use a local copy of the reply pointer to avoid race conditions
    QNetworkReply* reply = m_currentReply;
    m_currentReply = nullptr;  // Clear immediately to prevent double-free
    
    if (!reply) {
        qWarning() << "Search: reply was null after wait";
        return QVariantList();
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Search error: %1 (%2)")
            .arg(reply->errorString(),
                 QString::number(reply->error()));
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return QVariantList();
    }
    
    // Parse the response
    QByteArray data = reply->readAll();
    reply->deleteLater();
    
    return parseSearchResponse(data);
}

QVariantMap SearchClient::getItemMetadata(const QString& identifier)
{
    QUrl url(QString("https://archive.org/metadata/%1").arg(identifier));
    
    qDebug() << "IA Metadata:" << url.toString();
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent());
    request.setRawHeader("Accept", "application/json");
    
    m_currentReply = m_networkManager.get(request);
    
    // Synchronous wait
    QEventLoop loop;
    connect(m_currentReply, &QNetworkReply::finished, &loop, &QEventLoop::quit, Qt::UniqueConnection);
    loop.exec();
    
    // Use local pointer to avoid race conditions
    QNetworkReply* reply = m_currentReply;
    m_currentReply = nullptr;
    
    if (!reply) {
        qWarning() << "Metadata: reply was null after wait";
        return QVariantMap();
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Metadata error: %1 (%2)")
            .arg(reply->errorString(),
                 QString::number(reply->error()));
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return QVariantMap();
    }
    
    QByteArray data = reply->readAll();
    reply->deleteLater();
    
    return parseMetadataResponse(data);
}

void SearchClient::onSearchReplyFinished()
{
    if (m_currentReply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Search error: %1 (%2)")
            .arg(m_currentReply->errorString(),
                 QString::number(m_currentReply->error()));
        emit errorOccurred(errorMsg);
    } else {
        QByteArray data = m_currentReply->readAll();
        QVariantList results = parseSearchResponse(data);
        emit searchCompleted(results);
    }
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void SearchClient::onMetadataReplyFinished()
{
    if (m_currentReply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("Metadata error: %1 (%2)")
            .arg(m_currentReply->errorString(),
                 QString::number(m_currentReply->error()));
        emit errorOccurred(errorMsg);
    } else {
        QByteArray data = m_currentReply->readAll();
        QVariantMap metadata = parseMetadataResponse(data);
        emit itemMetadataReady(metadata);
    }
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

QVariantList SearchClient::parseSearchResponse(const QByteArray& data)
{
    QVariantList results;
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse IA search response:" << parseError.errorString();
        return results;
    }
    
    if (!doc.isObject()) {
        qWarning() << "IA search response is not a JSON object";
        return results;
    }
    
    QJsonObject root = doc.object();
    
    // IA API wraps results in a "response" object
    if (!root.contains("response")) {
        qWarning() << "IA search response missing 'response' field";
        return results;
    }
    
    QJsonObject response = root["response"].toObject();
    if (!response.contains("docs")) {
        qWarning() << "IA search response missing 'docs' field";
        return results;
    }
    
    QJsonArray docs = response["docs"].toArray();
    
    for (const QJsonValue& docVal : docs) {
        QJsonObject docObj = docVal.toObject();
        
        QVariantMap result;
        
        // Extract standard IA fields
        if (docObj.contains("identifier"))
            result["identifier"] = docObj["identifier"].toString();
        if (docObj.contains("title"))
            result["title"] = docObj["title"].toString();
        if (docObj.contains("mediatype"))
            result["mediatype"] = docObj["mediatype"].toString();
        if (docObj.contains("creator"))
            result["creator"] = docObj["creator"].toString();
        if (docObj.contains("date"))
            result["date"] = docObj["date"].toString();
        if (docObj.contains("description"))
            result["description"] = docObj["description"].toString();
        if (docObj.contains("subject")) {
            QJsonArray subjects = docObj["subject"].toArray();
            QStringList subjectList;
            for (const QJsonValue& s : subjects) {
                subjectList.append(s.toString());
            }
            result["subject"] = QVariant::fromValue(subjectList);
        }
        if (docObj.contains("collection")) {
            QJsonArray collections = docObj["collection"].toArray();
            QStringList collectionList;
            for (const QJsonValue& c : collections) {
                collectionList.append(c.toString());
            }
            result["collection"] = QVariant::fromValue(collectionList);
        }
        
        results.append(QVariant::fromValue(result));
    }
    
    qDebug() << "Parsed" << results.size() << "search results";
    return results;
}

QVariantMap SearchClient::parseMetadataResponse(const QByteArray& data)
{
    QVariantMap metadata;
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse IA metadata response:" << parseError.errorString();
        return metadata;
    }
    
    if (!doc.isObject()) {
        qWarning() << "IA metadata response is not a JSON object";
        return metadata;
    }
    
    QJsonObject root = doc.object();
    
    // Extract key metadata fields
    for (auto it = root.begin(); it != root.end(); ++it) {
        const QString& key = it.key();
        const QJsonValue& value = it.value();
        
        if (key == "dir") {
            // Convert file array to QVariantList of strings
            QVariantList files;
            if (value.isArray()) {
                for (const QJsonValue& f : value.toArray()) {
                    files.append(f.toString());
                }
            }
            metadata["files"] = QVariant::fromValue(files);
        } else if (key == "filesizes" || key == "formats" || key == "checksums") {
            // Convert maps to QVariantMap of strings
            QVariantMap strMap;
            if (value.isObject()) {
                for (auto mit = value.toObject().begin(); mit != value.toObject().end(); ++mit) {
                    strMap[mit.key()] = mit.value().toString();
                }
            }
            metadata[key] = QVariant::fromValue(strMap);
        } else if (value.isString()) {
            metadata[key] = value.toString();
        } else if (value.isArray()) {
            QStringList list;
            for (const QJsonValue& v : value.toArray()) {
                list.append(v.toString());
            }
            metadata[key] = QVariant::fromValue(list);
        } else if (value.isDouble()) {
            metadata[key] = value.toDouble();
        } else if (value.isBool()) {
            metadata[key] = value.toBool();
        }
    }
    
    qDebug() << "Parsed IA metadata with" << metadata.keys().size() << "fields";
    return metadata;
}
