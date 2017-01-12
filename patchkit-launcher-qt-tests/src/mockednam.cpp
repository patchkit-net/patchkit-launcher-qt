/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mockednam.h"

MockedNAM::MockedNAM()
    : QNetworkAccessManager()
{
}

void MockedNAM::push(QString t_url, QByteArray t_data, int t_replyDelay)
{
    MockedNetworkReply* reply = new MockedNetworkReply(t_replyDelay, t_data);

    m_Replies.insert(t_url, reply);
}

QNetworkReply* MockedNAM::createRequest(QNetworkAccessManager::Operation /*op*/, const QNetworkRequest& request, QIODevice* /*outgoingData*/)
{
    QString url = request.url().toString();

    if (!m_Replies.contains(url))
    {
        return nullptr;
    }

    MockedNetworkReply* reply = *m_Replies.find(url);

    reply->launch();

    return reply;
}
