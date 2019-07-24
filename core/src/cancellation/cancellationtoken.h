/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <memory>

#include "cancellationtokensource.h"
#include <customexceptions.h>

class CancellationToken : public QObject
{
    Q_OBJECT
public:
    CUSTOM_EXCEPTION(CancelledException, "Cancelled")

    CancellationToken(const CancellationToken& t_other)
        : QObject(t_other.parent())
        , m_source(t_other.m_source)
    {
        connect(&m_source, &CancellationTokenSource::cancelled, this, &CancellationToken::cancelled);
    }

    CancellationToken(CancellationTokenSource& t_source)
        : m_source(t_source)
    {
        connect(&m_source, &CancellationTokenSource::cancelled, this, &CancellationToken::cancelled);
    }

    bool isCancelled() const
    {
        return m_source.isCancelled();
    }

    void throwIfCancelled() const
    {
        if (isCancelled())
        {
            throw CancelledException();
        }
    }

    [[ noreturn ]]
    static void cancelHere()
    {
        throw CancelledException();
    }

signals:
    void cancelled();

private:
    CancellationTokenSource& m_source;
};
