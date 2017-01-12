/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mockednam.h"

MockedNAM::MockedNAM(QByteArray t_data)
    : QNetworkAccessManager()
    , m_data(t_data)
{
}

QNetworkReply* MockedNAM::createRequest(QNetworkAccessManager::Operation /*op*/, const QNetworkRequest& /*request*/, QIODevice* /*outgoingData*/)
{
    MockedNetworkReply* reply = new MockedNetworkReply();
    reply->setContent(m_data);
    return reply;
}
