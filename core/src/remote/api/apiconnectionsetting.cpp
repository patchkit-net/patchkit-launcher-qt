#include "apiconnectionsetting.h"

ApiConnectionSettings::ApiConnectionSettings(QString mainApiUrl, QStringList cacheApiUrls)
    : mainApiUrl(mainApiUrl)
    , cacheApiUrls(cacheApiUrls)
{
}
