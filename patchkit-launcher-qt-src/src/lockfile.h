/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QFile>

class LockFile
{
public:
    void lock();
    void unlock();

    bool isLocked() const;
    bool isLockedLocally() const;

    static LockFile& singleton();

private:
    LockFile();
    ~LockFile();

    QFile m_lockFile;
    bool m_locked;
};
