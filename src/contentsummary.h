/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CONTENTSUMMARY_H
#define CONTENTSUMMARY_H

#include <QString>
#include <QVector>

#include "hashingstrategy.h"

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
    const static QString   encryptionMethodToken;
    const static QString   compressionMethodToken;
    const static QString   hashingMethodToken;
    const static QString   hashCodeToken;
    const static QString   filesToken;
    const static QString   hashesToken;
    const static QString   hashToken;
    const static QString   pathToken;
    const static QString   chunksToken;
    const static QString   sizeToken;

    ContentSummary(const QJsonDocument& t_document);
    ContentSummary();

    /**
     * @brief getChunkHash
     * @param t_index
     *
     * The method returns the hash of a chunk with a specified index.
     * Will return 0 if the index is out of bounds.
     * Use the overloaded version which provides a mechanism for error detection.
     *
     * @return
     */
    const THash     getChunkHash(int t_index)                       const;
    const THash     getChunkHash(int t_index, bool& t_outOfBounds)  const;

    /**
     * @brief getFileData
     * @param t_index
     *
     * Returns information about a file (path and hash) with a specified index.
     * Will return FileData::dummy if the index is out of bounds.
     * Use the overloaded version which provides a mechanism for error detection.
     *
     * @return
     */
    const FileData& getFileData(int t_index)                        const;
    const FileData& getFileData(int t_index, bool& t_outOfBounds)   const;

    const bool      isValid()                const;
    const int       getChunkSize()           const;
    const QString&  getEncryptionMethod()    const;
    const QString&  getCompressionMethod()   const;
    const QString&  getHashingMethod()       const;
    const THash     getHashCode()            const;
    const int       getChunksCount()         const;

private:
    bool parseFiles(QJsonObject& t_document);
    bool parseChunks(QJsonObject& t_document);

    bool                m_isValid;
    int                 m_chunkSize;
    THash               m_hashCode;
    QString             m_encryptionMethod;
    QString             m_compressionMethod;
    QString             m_hashingMethod;
    QVector<THash>      m_chunkHashes;
    QVector<FileData>   m_filesSummary;
};

#endif // CONTENTSUMMARY_H
