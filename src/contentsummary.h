/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CONTENTSUMMARY_H
#define CONTENTSUMMARY_H

#include <QString>
#include <QStringList>

class QJsonDocument;
class QJsonObject;

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
    ContentSummary(const QJsonDocument& document);
    
    const int&      getChunkSize()           const;
    const QString&  getChunkHash(int at)     const;
    const QString&  getEncryptionMethod()    const;
    const QString&  getCompressionMethod()   const;
    const QString&  getHashingMethod()       const;
    const QString&  getHashCode()            const;
    const int&      getChunksCount()         const;
    
private:
    
    bool parseFiles(QJsonObject& doc);
    bool parseChunks(QJsonObject& doc);
    
    QString m_encryptionMethod;
    QString m_compressionMethod;
    QString m_hashingMethod;
    QString m_hashCode;
    
    int m_chunkSize;
    QStringList m_chunkHashes;
};

#endif // CONTENTSUMMARY_H
