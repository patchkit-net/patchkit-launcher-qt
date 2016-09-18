/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "cancellationtoken.h"

class Api : public QObject
{
    Q_OBJECT
public:
    explicit Api(QObject* parent = nullptr);

    QString downloadString(const QString& t_resourceUrl, CancellationToken t_cancellationToken) const;

private:
    QString downloadString(const QString& t_resourceUrl, QStringList& t_cacheApiUrls, bool t_extendedTimeout, CancellationToken t_cancellationToken) const;

    bool isVaild(int t_statusCode) const;

    bool downloadStringFromServer(const QString& t_url, int t_timeout, QString& t_result, int& t_statusCode, CancellationToken t_cancellationToken) const;
};
