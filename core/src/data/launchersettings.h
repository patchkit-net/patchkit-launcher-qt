#pragma once

#include <QSettings>

class LauncherSettings
{
public:
    LauncherSettings(QString appSecret);

    QString patcherSecret() const;
    void savePatcherSecret(const QString& newValue);

private:
    QSettings m_settings;
};
