#include "apiconnectionsetting.h"
#include "config.h"
#include <QProcessEnvironment>

using namespace remote::api;

ApiConnectionSettings::ApiConnectionSettings(QString mainApiUrl, QStringList cacheApiUrls)
    : mainApiUrl(mainApiUrl)
    , cacheApiUrls(cacheApiUrls)
{
}

remote::api::ApiConnectionSettings remote::api::defaultConnectionSettings()
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
