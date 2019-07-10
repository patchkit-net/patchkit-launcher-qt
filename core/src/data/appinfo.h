#pragma once

#include <QString>
#include <QJsonDocument>

struct AppInfo
{
public:
    static AppInfo parse(const QJsonDocument& document);

    QString displayName;
    QString author;
};
