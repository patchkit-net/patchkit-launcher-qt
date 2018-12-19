/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "tempdir.h"

#include <QDir>

TempDir::TempDir(const QString& t_path) : m_path(t_path)
{
    create();
}

TempDir::~TempDir()
{
    remove();
}

void TempDir::create() const
{
    QDir dir(m_path);

    QFileInfo info(m_path);

    if (info.isDir())
    {
        dir.removeRecursively();
    }
    else if (info.isFile())
    {
        QFile::remove(m_path);
    }

    dir.mkpath(".");
}

void TempDir::remove() const
{
    QDir dir(m_path);

    dir.removeRecursively();
}
