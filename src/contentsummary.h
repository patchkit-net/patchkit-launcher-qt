/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CONTENTSUMMARY_H
#define CONTENTSUMMARY_H

#include <QString>
#include <QVector>

class QJsonDocument;
class QJsonObject;

typedef unsigned int THash;

/**
 * @brief The ContentSummary class
 *
 * Reflects the JSON contatining crucial information about file.
 *
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
    ContentSummary(const QJsonDocument& document);
    ContentSummary();

    const bool      isValid()                const;
    const int       getChunkSize()           const;
    const THash     getChunkHash(int at)     const;
    const QString&  getEncryptionMethod()    const;
    const QString&  getCompressionMethod()   const;
    const QString&  getHashingMethod()       const;
    const THash     getHashCode()            const;
    const int       getChunksCount()         const;

private:
    bool parseFiles(QJsonObject& doc);
    bool parseChunks(QJsonObject& doc);

    void invalidate();

    bool    m_isValid;
    QString m_encryptionMethod;
    QString m_compressionMethod;
    QString m_hashingMethod;
    THash   m_hashCode;
    int     m_chunkSize;
    QVector<THash> m_chunkHashes;
};

#endif // CONTENTSUMMARY_H
