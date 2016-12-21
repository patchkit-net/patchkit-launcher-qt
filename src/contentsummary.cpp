/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "contentsummary.h"

#include "logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QStringList>
#include <QVariantList>

ContentSummary::ContentSummary(const QJsonDocument& document)
{
    const std::string PARSING_ERROR_MSG = "Couldn't parse content summary from provided JSON document";

    if (document.isEmpty() || document.isNull())
    {
        throw std::runtime_error("Provided JSON document is invalid.");
    }

    if (!document.isObject())
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }

    QJsonObject& doc_object = document.object();

    if (!doc_object.contains("encryption_method"))
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }

    m_encryptionMethod = doc_object["encryption_method"].toString();

    if (!doc_object.contains("compression_method"))
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }

    m_compressionMethod = doc_object["compression_method"].toString();

    if (!doc_object.contains("hashing_method"))
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }

    m_hashingMethod = doc_object["hashing_method"].toString();

    if (!doc_object.contains("hash_code"))
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }

    m_hashCode = doc_object["hash_code"].toString();

    if (!parseFiles(doc_object))
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }

    if (!parseChunks(doc_object))
    {
        throw std::runtime_error(PARSING_ERROR_MSG);
    }
}

const int& ContentSummary::getChunkSize() const
{
    return m_chunkSize;
}

const THash ContentSummary::getChunkHash(int at) const
{
    bool ok;
    THash hash = m_chunkHashes.at(at).toUInt(&ok, 16);
    return hash;
}

const QString& ContentSummary::getEncryptionMethod() const
{
    return m_encryptionMethod;
}

const QString& ContentSummary::getCompressionMethod() const
{
    return m_compressionMethod;
}

const QString& ContentSummary::getHashingMethod() const
{
    return m_hashingMethod;
}

const THash ContentSummary::getHashCode() const
{
    bool ok;
    THash hash = m_hashCode.toUInt(&ok, 16);
    return hash;
}

const int& ContentSummary::getChunksCount() const
{
    return m_chunkHashes.size();
}

bool ContentSummary::parseFiles(QJsonObject& doc)
{
    if (!doc.contains("files"))
    {
        return false;
    }

    QJsonArray files = doc["files"].toArray();

    for (QJsonValueRef f : files)
    {
        /* TODO implement parsing files. */
    }

    return true;
}

bool ContentSummary::parseChunks(QJsonObject& doc)
{
    if (!doc.contains("chunks"))
    {
        return false;
    }

    QJsonObject chunks = doc["chunks"].toObject();

    if (chunks == QJsonObject())
    {
        return false;
    }

    int chunks_size = chunks["size"].toInt(-1);

    if (chunks_size == -1)
    {
        return false;
    }

    if (!chunks.contains("hashes"))
    {
        return false;
    }

    QJsonArray hashes = chunks["hashes"].toArray();

    if (hashes == QJsonArray())
    {
        return false;
    }

    QStringList hash_list;

    for (QJsonValueRef item : hashes)
    {
        hash_list.push_back(item.toString());
    }

    m_chunkSize = chunks_size;
    m_chunkHashes = hash_list;

    return true;
}
