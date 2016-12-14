/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CONTENTSUMMARY_H
#define CONTENTSUMMARY_H

#include <QString>
#include <QStringList>

class QJsonDocument;

/**
 * @brief The ContentSummary class
 * 
 * Reflects the JSON contatining crucial information about file.
 * 
 * Refer to:
 * redmine.patchkit.net/projects/patchkit-documentation/wiki/Content_Summary
 * for more detailed info.
 */
class ContentSummary
{
public:
    ContentSummary(const QJsonDocument& document);
    
    int GetChunksSize() const;
    QString GetChunk(int at) const;
    
private:
    int m_chunksSize;
    QStringList m_chunkHashes;
};

#endif // CONTENTSUMMARY_H
