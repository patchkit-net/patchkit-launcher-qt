/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include <QLockFile>
#include <QFile>

class LockFile : public QObject
{
    Q_OBJECT
public:
    explicit LockFile(QObject* parent = nullptr);
    ~LockFile();

    void lock();
    void unlock();

    void cede();

private:
    QLockFile m_lockFile;
    bool m_isLockFileLocal;
};
