/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>
#include <QJsonDocument>

class PatcherManifest;

namespace Utilities
{
    template<typename T>
    struct Maybe
    {
        Maybe(bool is, T value)
            : is(is), value(value)
        {}

        bool is;
        T value;
    };

    bool isCurrentDirectoryWritable();

    void tryRestartWithHigherPermissions();

    PatcherManifest parsePatcherManifest(const QString& t_filename);
    PatcherManifest parsePatcherManifest(const QJsonDocument& t_document);

    PatcherManifest parseOldPatcherManifest(const QString& t_filename);
    PatcherManifest parseOldPatcherManifest(const QJsonDocument& t_document);
}
