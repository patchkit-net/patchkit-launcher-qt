#include "launchersettings.h"

LauncherSettings::LauncherSettings(QString appSecret)
    : m_settings("UpSoft", appSecret.append("launcher-"))
{
}

QString LauncherSettings::patcherSecret() const
{
    return m_settings.value("patcher_secret").toString();
}

void LauncherSettings::savePatcherSecret(const QString& newValue)
{
    m_settings.setValue("patcher_secret", newValue);
}
