#ifndef REMOTEDATAREPLY_H
#define REMOTEDATAREPLY_H

#include <QtNetwork>

class RemoteDataReply : public QNetworkReply
{
    Q_OBJECT
public:

    virtual QByteArray readAll();
    virtual QNetworkReply::NetworkError error() const;
    virtual QString errorString() const;
    virtual QVariant attribute(QNetworkRequest::Attribute code) const;
    virtual bool isFinished() const;
    virtual bool waitForReadyRead(int msecs);

public slots:
    virtual void deleteLater();
    virtual void abort();

};

#endif // REMOTEDATAREPLY_H
