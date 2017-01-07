#include "remotedatareply.h"

QByteArray RemoteDataReply::readAll()
{
    return QNetworkReply::readAll();
}

QNetworkReply::NetworkError RemoteDataReply::error() const
{
    return QNetworkReply::error();
}

QString RemoteDataReply::errorString() const
{
    return QNetworkReply::errorString();
}

QVariant RemoteDataReply::attribute(QNetworkRequest::Attribute code) const
{
    return QNetworkReply::attribute(code);
}

bool RemoteDataReply::isFinished() const
{
    return QNetworkReply::isFinished();
}

bool RemoteDataReply::waitForReadyRead(int msecs)
{
    return QNetworkReply::waitForReadyRead(msecs);
}

void RemoteDataReply::deleteLater()
{
    QNetworkReply::deleteLater();
}

void RemoteDataReply::abort()
{
}
