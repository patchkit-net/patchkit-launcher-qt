#ifndef IAPI_H
#define IAPI_H

class IApi
{
public:
    virtual QString downloadString(const QString& t_resourceUrl, CancellationToken t_cancellationToken) const = 0;
    virtual QJsonDocument downloadContentSummary(const QString& t_resourceUrl, CancellationToken t_cancellationToken) const = 0;
};

#endif // IAPI_H
