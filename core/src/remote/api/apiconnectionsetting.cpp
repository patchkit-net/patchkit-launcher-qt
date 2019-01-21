#include "apiconnectionsetting.h"
#include "config.h"
#include <QProcessEnvironment>

namespace remote
{
namespace api
{

ApiConnectionSettings::ApiConnectionSettings(QString mainApiUrl, QStringList cacheApiUrls)
    : mainApiUrl(mainApiUrl)
    , cacheApiUrls(cacheApiUrls)
{
}

ApiConnectionSettings defaultConnectionSettings()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString mainApiUrl = Config::mainApiUrl;
    QStringList cacheApiUrls = Config::cacheApiUrls;

    if (env.contains(Config::apiUrlOverrideEnvironmentVariableName))
    {
        mainApiUrl = env.value(Config::apiUrlOverrideEnvironmentVariableName);
    }

    if (env.contains(Config::apiCacheUrlOverrideEnvironmentVariableName))
    {
        QString cacheApiUrl = env.value(Config::apiCacheUrlOverrideEnvironmentVariableName);
        cacheApiUrls = QStringList(cacheApiUrl);
    }

    return ApiConnectionSettings(mainApiUrl, cacheApiUrls);
}

} // namespace api
} // namespace remote
