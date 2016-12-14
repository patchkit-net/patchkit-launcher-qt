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
    if (document.isEmpty() || document.isNull())
        throw std::runtime_error("Couldn't resolve provided data to a JSON document.");
    
    if (!document.isObject())
        throw std::runtime_error("Invalid content sumarry structure.");

    QJsonObject base = document.object();
    
    QJsonObject chunks = base["chunks"].toObject();
    
    m_chunksSize = chunks["size"].toInt();
    
    QVariantList variantList = chunks["hashes"].toArray().toVariantList();
    
    for (QVariant variant : variantList)
    {
        if (variant.type() == QVariant::String)
            m_chunkHashes.push_back(variant.toString());
        else
            throw std::runtime_error("Unexpected variant type.");
    }
}

int ContentSummary::GetChunksSize() const
{
    return m_chunksSize;
}

QString ContentSummary::GetChunk(int at) const
{
    return m_chunkHashes.at(at);
}
