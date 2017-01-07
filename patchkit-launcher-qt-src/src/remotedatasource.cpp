#include "remotedatasource.h"

RemoteDataSource::RemoteDataSource()
    : m_networkAccessManager(new QNetworkAccessManager())
{
}

RemoteDataReply* RemoteDataSource::get(const QNetworkRequest& t_request)
{
    return (RemoteDataReply*) m_networkAccessManager->get(t_request);
}
