#include "appinfo.h"

#include "customexceptions.h"
#include <QJsonObject>
#include <QVariant>

static const QString displayNameKey = "display_name";
static const QString authorKey = "author";

AppInfo AppInfo::parse(const QJsonDocument& document)
{
    if (document.isEmpty() || document.isNull())
    {
        throw InvalidFormatException("AppInfo must not be empty");
    }
    if (!document.isObject()) {
        throw InvalidFormatException("AppInfo root must be object");
    }

    QString displayName;
    QString author;

    QJsonObject object = document.object();

    if (object.contains(displayNameKey))
    {
        auto variant = object.value(displayNameKey);
        if (variant.isString())
        {
            displayName = variant.toString();
        }
        else if (!variant.isNull())
        {
            throw InvalidFormatException("display_name must be string or null");
        }
    }

    if (object.contains(authorKey))
    {
        auto variant = object.value(authorKey);
        if (variant.isString())
        {
            author = variant.toString();
        }
        else if (!variant.isNull())
        {
            throw InvalidFormatException("display_name must be string or null");
        }
    }

    return AppInfo { displayName , author };
}
