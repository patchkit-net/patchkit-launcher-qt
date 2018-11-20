#pragma once
#include <QString>
#include <QStringList>

struct ApiConnectionSettings
{
    ApiConnectionSettings(QString mainApiUrl, QStringList cacheApiUrls);

    ApiConnectionSettings(const ApiConnectionSettings&) = default;

    QString mainApiUrl;
    QStringList cacheApiUrls;
};
