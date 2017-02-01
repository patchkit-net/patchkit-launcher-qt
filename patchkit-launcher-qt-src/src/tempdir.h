/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>

class TempDir
{
public:
    TempDir(const QString& t_path);

    ~TempDir();

private:
    void create() const;

    void remove() const;

    QString m_path;
};
