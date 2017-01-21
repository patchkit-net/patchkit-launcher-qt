/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mockednam.h"

MockedNAM::MockedNAM()
    : QNetworkAccessManager()
    , m_shouldCorruptNext(false)
{
}

void MockedNAM::push(QString t_url, QByteArray t_data, int t_replyDelay)
{
    m_ReplyDefinitions.insert(t_url, ReplyDefinition(t_data, t_replyDelay));
}

void MockedNAM::corruptNext()
{
    m_shouldCorruptNext = true;
}

QNetworkReply* MockedNAM::createRequest(QNetworkAccessManager::Operation /*op*/, const QNetworkRequest& request, QIODevice* /*outgoingData*/)
{
    QString url = request.url().toString();

    qint64 offset = 0;

    if (request.hasRawHeader("Range"))
    {
        offset = parseRangeHeader(request.rawHeader("Range"));
    }

    if (!m_ReplyDefinitions.contains(url))
    {
        return nullptr;
    }

    ReplyDefinition& def = *m_ReplyDefinitions.find(url);

    MockedNetworkReply* reply = new MockedNetworkReply(def.delay, def.data);

    reply->setOffset(offset);

    if (m_shouldCorruptNext)
    {
        m_shouldCorruptNext = false;
        reply->corrupt();
    }

    reply->launch();

    return reply;
}

qint64 MockedNAM::parseRangeHeader(const QByteArray &t_rangeHeader)
{
    if (t_rangeHeader.size() == 0)
    {
        return 0;
    }

    // Header is formulated as so: "bytes=start-end"
    // We only care about start so it'll be assumed the header is of form "bytes=start-"

    QString header (t_rangeHeader);

    QStringList tokens = header.split('=');

    if (tokens.size() < 2)
    {
        return 0;
    }

    QString offsetString = tokens.at(1);
    offsetString = offsetString.mid(0, offsetString.size() - 1);

    return offsetString.toLongLong();
}

