/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <boost/interprocess/sync/file_lock.hpp>

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
    boost::interprocess::file_lock m_lockFile;
    bool m_isLockFileLocal;
};
