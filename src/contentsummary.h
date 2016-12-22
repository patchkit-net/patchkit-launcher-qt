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
    /**
     * @brief ContentSummary
     * @param document - A valid json document containing necessary data.
     *
     * After being constructed the ContentSummary object is immutable.
     */
    ContentSummary(const QJsonDocument& t_document);
    ContentSummary();

    const bool      isValid()                const;
    const int       getChunkSize()           const;
    const THash     getChunkHash(int t_at)   const;
    const QString&  getEncryptionMethod()    const;
    const QString&  getCompressionMethod()   const;
    const QString&  getHashingMethod()       const;
    const THash     getHashCode()            const;
    const int       getChunksCount()         const;
    const FileData& getFileData(int t_at)    const;

private:
    bool parseFiles(QJsonObject& t_document);
    bool parseChunks(QJsonObject& t_document);

    bool                m_isValid;
    QString             m_encryptionMethod;
    QString             m_compressionMethod;
    QString             m_hashingMethod;
    THash               m_hashCode;
    int                 m_chunkSize;
    QVector<THash>      m_chunkHashes;
    QVector<FileData>   m_filesSummary;
};

#endif // CONTENTSUMMARY_H
