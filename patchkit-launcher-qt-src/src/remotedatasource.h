#ifndef REMOTEDATASOURCE_H
#define REMOTEDATASOURCE_H

#include <QtNetwork>
#include "remotedatareply.h"

class RemoteDataReply;

class RemoteDataSource
{
public:
    RemoteDataSource();

    virtual RemoteDataReply* get(const QNetworkRequest& t_request);

private:
    QSharedPointer<QNetworkAccessManager> m_networkAccessManager;
};

#endif // REMOTEDATASOURCE_H
