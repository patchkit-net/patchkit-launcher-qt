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

    enum class Response
    {
        None, Proceed, Stop
    };

    State getState() const;
    void setState(State t_state);

    Response awaitResponseTo(DownloadError t_error, CancellationToken t_cancellationToken);

signals:
    void error(DownloadError t_error);
    void respond(Response t_response);

private slots:
    void respondInternal(Response t_response);

private:
    State m_state;
    Response m_response;
};
