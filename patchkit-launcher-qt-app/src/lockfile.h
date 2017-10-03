/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QFile>

class LockFile
{
public:
    LockFile();
    ~LockFile();

    void lock();
    void unlock();

    bool isLocked() const;

private:
    QFile m_lockFile;
    bool m_locked;
};
