/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef MOCKEDNAM_H
#define MOCKEDNAM_H

#include "mockedreply.h"

#include <QNetworkAccessManager>
#include <QtNetwork>

class MockedNAM : public QNetworkAccessManager
{
public:
    MockedNAM(QByteArray t_data);

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest& request, QIODevice* outgoingData) override;

private:
    QByteArray m_data;
};

#endif // MOCKEDNAM_H
