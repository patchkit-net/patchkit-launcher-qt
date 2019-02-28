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

const QString ContentSummary::encryptionMethodToken   = QString("encryption_method");
const QString ContentSummary::compressionMethodToken  = QString("compression_method");
const QString ContentSummary::hashingMethodToken      = QString("hashing_method");
const QString ContentSummary::hashCodeToken           = QString("hash_code");
const QString ContentSummary::filesToken              = QString("files");
const QString ContentSummary::hashesToken             = QString("hashes");
const QString ContentSummary::hashToken               = QString("hash");
const QString ContentSummary::pathToken               = QString("path");
const QString ContentSummary::chunksToken             = QString("chunks");
const QString ContentSummary::chunkSizeToken          = QString("size");
const QString ContentSummary::sizeToken               = QString("size");
const QString ContentSummary::uncompressedSizeToken   = QString("uncompressed_size");

const FileData& FileData::dummy = FileData("dummy", 0);

ContentSummary::InvalidFormat::InvalidFormat(const std::string &message)
    : InvalidFormatException (std::string("ContentSummary: ") + message)
{
}


ContentSummary::ContentSummary(int t_chunkSize, THash t_hashCode
                               , QString t_encryptionMethod
                               , QString t_compressionMethod
                               , QString t_hashingMethod, QVector<THash> t_chunkHashes
                               , QVector<FileData> t_filesSummary
                               , int t_size, int t_uncompressedSize)
    : m_chunkSize(t_chunkSize)
    , m_uncompressedSize(t_uncompressedSize)
    , m_size(t_size)
    , m_hashCode(t_hashCode)
    , m_encryptionMethod(t_encryptionMethod)
    , m_compressionMethod(t_compressionMethod)
    , m_hashingMethod(t_hashingMethod)
    , m_chunkHashes(t_chunkHashes)
    , m_filesSummary(t_filesSummary)
{
}

ContentSummary ContentSummary::fromData(
        const QByteArray& t_data, int t_chunkSize, int t_size,
        int t_uncompressedSize, HashFunc t_hashingMethod,
        THash t_hashCode, QString t_hashingMethodName)
{
    QVector<THash> hashes;
    for (int i = 0; i < t_data.size(); i += t_chunkSize)
    {
        hashes.push_back(t_hashingMethod(t_data.mid(i, t_chunkSize)));
    }

    return ContentSummary(
                t_chunkSize, t_hashCode,
                "none", "none", t_hashingMethodName,
                hashes, {}, t_size, t_uncompressedSize);
}

ContentSummary::ContentSummary(const QJsonDocument& t_document)
{
    if (t_document.isEmpty() || t_document.isNull())
    {
        throw InvalidFormat("Document is empty");
    }

    if (!t_document.isObject())
    {
        throw InvalidFormat("Document is not an object");
    }

    QJsonObject doc_object = t_document.object();

    if (!doc_object.contains(encryptionMethodToken))
    {
        throw InvalidFormat("Document doesn't contain an encryption method");
    }

    m_encryptionMethod = doc_object[encryptionMethodToken].toString();

    if (!doc_object.contains(compressionMethodToken))
    {
        throw InvalidFormat("Document doesn't contain a compression method");
    }

    m_compressionMethod = doc_object[compressionMethodToken].toString();

    if (!doc_object.contains(hashingMethodToken))
    {
        throw InvalidFormat("Document doesn't contain a hashing method");
    }

    m_hashingMethod = doc_object[hashingMethodToken].toString();

    if (!doc_object.contains(hashCodeToken))
    {
        throw InvalidFormat("Document doesn't contain a hash code");
    }

    bool ok;
    m_hashCode = doc_object[hashCodeToken].toString().toUInt(&ok, 16);
    if (!ok)
    {
        throw InvalidFormat("Hash code format is invalid");
    }

    m_uncompressedSize = doc_object[uncompressedSizeToken].toInt(-1);
    if (m_uncompressedSize == -1)
    {
        throw InvalidFormat("Document doesn't contain uncompressed size");
    }

    m_size = doc_object[sizeToken].toInt(-1);
    if (m_size == -1)
    {
        throw InvalidFormat("Document doesn't contain size");
    }

    parseFiles(doc_object);
    parseChunks(doc_object);
}

int ContentSummary::getChunkSize() const
{
    return m_chunkSize;
}

THash ContentSummary::getChunkHash(int t_index, bool& t_outOfBounds) const
{
    t_outOfBounds = t_index < 0 || t_index >= m_chunkHashes.size();

    if (t_outOfBounds)
    {
        qCritical("Accessing a chunk hash out of bounds");
        return 0;
    }
    else
    {
        return m_chunkHashes.at(t_index);
    }
}

THash ContentSummary::getChunkHash(int t_index) const
{
    bool check;
    return getChunkHash(t_index, check);
}

const FileData& ContentSummary::getFileData(int t_index, bool& t_outOfBounds) const
{
    t_outOfBounds = t_index < 0 || t_index >= m_filesSummary.size();

    if (t_outOfBounds)
    {
        qCritical("Accessing file data out of bounds");
        return FileData::dummy;
    }
    else
    {
        return m_filesSummary.at(t_index);
    }
}

const FileData& ContentSummary::getFileData(int t_index) const
{
    bool check;
    return getFileData(t_index, check);
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

int ContentSummary::getCompressedSize() const
{
    return m_size;
}

int ContentSummary::getUncompressedSize() const
{
    return m_uncompressedSize;
}

THash ContentSummary::getHashCode() const
{
    return m_hashCode;
}

int ContentSummary::getChunksCount() const
{
    return m_chunkHashes.size();
}

int ContentSummary::getFilesCount() const
{
    return m_filesSummary.size();
}

QJsonDocument ContentSummary::toJson() const
{
    QJsonObject root;

    root[encryptionMethodToken] = getEncryptionMethod();
    root[compressionMethodToken] = getCompressionMethod();
    root[hashingMethodToken] = getHashingMethod();
    root[hashCodeToken] = QString::number(getHashCode(), 16);
    root[uncompressedSizeToken] = getUncompressedSize();
    root[sizeToken] = getCompressedSize();

    QJsonArray filesArray;

    for (const FileData& fileData : m_filesSummary)
    {
        QJsonObject fileObject;
        fileObject[pathToken] = fileData.path;
        fileObject[hashToken] = QString::number(fileData.hash, 16);

        filesArray.append(fileObject);
    }

    QJsonObject chunksObject;
    QJsonArray chunkHashes;

    for (THash hash : m_chunkHashes)
    {
        chunkHashes.append(QString::number(hash, 16));
    }

    chunksObject[chunkSizeToken] = getChunkSize();
    chunksObject[hashesToken] = chunkHashes;

    root[chunksToken] = chunksObject;
    root[filesToken] = filesArray;

    return QJsonDocument(root);
}

void ContentSummary::parseFiles(QJsonObject& t_document)
{
    if (!t_document.contains(filesToken))
    {
        throw InvalidFormat("Document doesn't contain files");
    }

    QJsonArray files = t_document[filesToken].toArray();

    bool ok;
    for (QJsonValueRef f : files)
    {
        if (!f.isObject())
        {
            throw InvalidFormat("Failed to parse file - entry wasn't an object");
        }

        if (!(f.toObject().contains(hashToken) && f.toObject().contains(pathToken)))
        {
            throw InvalidFormat("Failed to parse file - entry didn't contain hash or path");
        }

        QString path = f.toObject()[pathToken].toString();
        THash hash = f.toObject()[hashToken].toString().toUInt(&ok, 16);

        if (!ok)
        {
            throw InvalidFormat("Failed to parse file - entry hash was invalid");
        }

        m_filesSummary.push_back(FileData(path, hash));
    }
}

void ContentSummary::parseChunks(QJsonObject& t_document)
{
    if (!t_document.contains(chunksToken))
    {
        throw InvalidFormat("Document doesn't contain chunks");
    }

    QJsonObject chunks = t_document[chunksToken].toObject();

    if (chunks == QJsonObject())
    {
        throw InvalidFormat("Chunks are empty");
    }

    int chunks_size = chunks[chunkSizeToken].toInt(-1);

    if (chunks_size == -1)
    {
        throw InvalidFormat("Chunk size was missing or invalid");
    }

    if (!chunks.contains(hashesToken))
    {
        throw InvalidFormat("Document doesn't contain hashes");
    }

    QJsonArray hashes = chunks[hashesToken].toArray();

    if (hashes == QJsonArray())
    {
        throw InvalidFormat("Chunk hashes were invalid or empty");
    }

    QVector<THash> hash_list;

    bool ok;
    for (QJsonValueRef item : hashes)
    {
        hash_list.push_back(item.toString().toUInt(&ok, 16));
        if (!ok)
        {
            throw InvalidFormat("Failed to parse chunk - hash was missing or empty");
        }
    }

    m_chunkSize = chunks_size;
    m_chunkHashes = hash_list;
}
