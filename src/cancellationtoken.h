/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <memory>

#include "cancellationtokensource.h"
#include "cancelledexception.h"

class CancellationToken : public QObject
{
    Q_OBJECT
public:
    CancellationToken(const CancellationToken& t_other) :
        m_source(t_other.m_source)
    {
        connect(m_source.get(), &CancellationTokenSource::cancelled, this, &CancellationToken::cancelled);
    }

    CancellationToken(std::shared_ptr<CancellationTokenSource> t_source):
        m_source(t_source)
    {
        connect(m_source.get(), &CancellationTokenSource::cancelled, this, &CancellationToken::cancelled);
    }

    bool isCancelled() const
    {
        return m_source->isCancelled();
    }

    void throwIfCancelled() const
    {
        if (isCancelled())
        {
            throw CancelledException();
        }
    }

signals:
    void cancelled();

private:
    std::shared_ptr<CancellationTokenSource> m_source;
};
