/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "downloader.h"

class LauncherState : public QObject
{
    Q_OBJECT
public:
    explicit LauncherState(QObject* parent = nullptr);

    enum State
    {
        Initializing, Running, Paused, Stopped, OnError
    };

    State getState() const;
    void setState(State t_state);

    void awaitResponseTo(DownloadError t_error, CancellationToken t_cancellationToken);

signals:
    void error(DownloadError t_error);
    void respond();

private:
    State m_state;
};
