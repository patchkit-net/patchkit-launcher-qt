/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QLockFile>
#include <QFile>

class LockFile
{
public:
    LockFile();
    ~LockFile();

    void lock();
    void unlock();

    void cede();
    void clear();

private:
    QLockFile m_lockFile;
    bool m_isLockFileLocal;
};
