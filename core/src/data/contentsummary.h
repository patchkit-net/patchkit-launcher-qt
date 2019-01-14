/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>
#include <QVector>

#include "../hashingstrategy.h"

class QJsonDocument;
class QJsonObject;

struct FileData
{
    FileData(QString t_path, THash t_hash)
        : path(t_path)
        , hash(t_hash)
    {
    }

    FileData() = default;

    QString path;
    THash hash;

    const static FileData& dummy;
};

/**
 * @brief The ContentSummary class
 *
 * Reflects the JSON contatining crucial information about file.
 *
 * @note
 * Refer to:
 * http://redmine.patchkit.net/projects/patchkit-documentation/wiki/Content_Summary
 * for more detailed info.
 */
class ContentSummary
{
public:
    const static QString encryptionMethodToken;
    const static QString compressionMethodToken;
    const static QString hashingMethodToken;
    const static QString hashCodeToken;
    const static QString filesToken;
    const static QString hashesToken;
    const static QString hashToken;
    const static QString pathToken;
    const static QString chunksToken;
    const static QString chunkSizeToken;
    const static QString sizeToken;
    const static QString uncompressedSizeToken;

    static ContentSummary fromData(
            const QByteArray& t_data,
            int t_chunkSize, int t_size,
            int t_uncompressedSize,
            HashFunc t_hashingMethod,
            THash t_hashCode = 0,
            QString t_hashingMethodName = "xxHash");

    ContentSummary(const QJsonDocument& t_document);
    ContentSummary();

    ContentSummary(int t_chunkSize, THash t_hashCode, QString t_encryptionMethod
                   , QString t_compressionMethod, QString t_hashingMethod
                   , QVector<THash> t_chunkHashes, QVector<FileData> t_filesSummary
                   , int t_size, int t_uncompressedSize);

    /**
     * @brief getChunkHash
     * @param t_index
     *
     * The method returns the hash of a chunk with a specified index.
     * Will return 0 if the index is out of bounds.
     * For error checking use the overloaded version.
     *
     * @return
     */
    THash     getChunkHash(int t_index)                       const;
    THash     getChunkHash(int t_index, bool& t_outOfBounds)  const;

    /**
     * @brief getFileData
     * @param t_index
     *
     * Returns information about a file (path and hash) with a specified index.
     * Will return FileData::dummy if the index is out of bounds.
     * For error checking use the overloaded version.
     *
     * @return
     */
    const FileData& getFileData(int t_index)                        const;
    const FileData& getFileData(int t_index, bool& t_outOfBounds)   const;

    bool            isValid()                const;
    int             getChunkSize()           const;
    THash           getHashCode()            const;
    int             getChunksCount()         const;
    int             getFilesCount()          const;
    const QString&  getEncryptionMethod()    const;
    const QString&  getCompressionMethod()   const;
    const QString&  getHashingMethod()       const;
    int             getCompressedSize()      const;
    int             getUncompressedSize()    const;

    QJsonDocument   toJson() const;

private:
    bool parseFiles(QJsonObject& t_document);
    bool parseChunks(QJsonObject& t_document);

    bool                m_isValid;
    int                 m_chunkSize;
    int                 m_uncompressedSize;
    int                 m_size;
    THash               m_hashCode;
    QString             m_encryptionMethod;
    QString             m_compressionMethod;
    QString             m_hashingMethod;
    QVector<THash>      m_chunkHashes;
    QVector<FileData>   m_filesSummary;
};
