/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

class CancellationTokenSource : public QObject
{
    Q_OBJECT

public:
    CancellationTokenSource() : m_isCancelled(false)
    {
    }

    bool isCancelled() const
    {
        return m_isCancelled;
    }

    void cancel()
    {
        if (!m_isCancelled)
        {
            m_isCancelled = true;
            emit cancelled();
        }
    }

signals:
    void cancelled();

private:
    bool m_isCancelled;
};
