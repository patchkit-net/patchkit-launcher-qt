/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef MOCKEDNAM_H
#define MOCKEDNAM_H

#include "mockedreply.h"

#include <QNetworkAccessManager>
#include <QtNetwork>
#include <QMap>

class MockedNAM : public QNetworkAccessManager
{
public:
    MockedNAM();

    void push(QString t_url, QByteArray t_data, int t_replyDelay);

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData) override;

private:
    QByteArray m_data;
    int m_replyDelay;

    QMap<QString, MockedNetworkReply*> m_Replies;
};

#endif // MOCKEDNAM_H
