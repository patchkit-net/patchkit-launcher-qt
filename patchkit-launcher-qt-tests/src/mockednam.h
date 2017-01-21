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
    void corruptNext();

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData) override;

private:
    struct ReplyDefinition
    {
        ReplyDefinition(QByteArray t_data, int t_delay)
            : data(t_data)
            , delay(t_delay)
        {
        }

        QByteArray data;
        int delay;
    };

    QByteArray m_data;
    int m_replyDelay;

    bool m_shouldCorruptNext;

    qint64 parseRangeHeader(const QByteArray& t_rangeHeader);

    QMap<QString, ReplyDefinition> m_ReplyDefinitions;
};

#endif // MOCKEDNAM_H
