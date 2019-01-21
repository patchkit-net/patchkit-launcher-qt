#pragma once
#include <QString>
#include <QStringList>

namespace remote
{
namespace api
{
struct ApiConnectionSettings
{
    ApiConnectionSettings(QString mainApiUrl, QStringList cacheApiUrls);

    ApiConnectionSettings(const ApiConnectionSettings&) = default;

    QString mainApiUrl;
    QStringList cacheApiUrls;
};

ApiConnectionSettings defaultConnectionSettings();

} // namespace api
} // namespace remote
