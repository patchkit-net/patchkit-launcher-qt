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

ContentSummary::ContentSummary()
    : m_isValid(false)
{
}

ContentSummary::ContentSummary(const QJsonDocument& document)
    : m_isValid(false)
{
    if (document.isEmpty() || document.isNull())
    {
        return;
    }

    if (!document.isObject())
    {
        return;
    }

    QJsonObject& doc_object = document.object();

    if (!doc_object.contains("encryption_method"))
    {
        return;
    }

    m_encryptionMethod = doc_object["encryption_method"].toString();

    if (!doc_object.contains("compression_method"))
    {
        return;
    }

    m_compressionMethod = doc_object["compression_method"].toString();

    if (!doc_object.contains("hashing_method"))
    {
        return;
    }

    m_hashingMethod = doc_object["hashing_method"].toString();

    if (!doc_object.contains("hash_code"))
    {
        return;
    }

    bool ok;
    m_hashCode = doc_object["hash_code"].toString().toUInt(&ok, 16);

    if (!ok)
    {
        return;
    }

    if (!parseFiles(doc_object))
    {
        return;
    }

    if (!parseChunks(doc_object))
    {
        return;
    }

    m_isValid = true;
}

const bool ContentSummary::isValid() const
{
    return m_isValid;
}

const int ContentSummary::getChunkSize() const
{
    return m_chunkSize;
}

const THash& ContentSummary::getChunkHash(int at) const
{
    return m_chunkHashes.at(at);
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

const THash& ContentSummary::getHashCode() const
{
    return m_hashCode;
}

const int ContentSummary::getChunksCount() const
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

    QVector<THash> hash_list;

    bool ok;
    for (QJsonValueRef item : hashes)
    {
        hash_list.push_back(item.toString().toUInt(&ok, 16));
        if (!ok)
        {
            return false;
        }
    }

    m_chunkSize = chunks_size;
    m_chunkHashes = hash_list;

    return true;
}
