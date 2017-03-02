/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "cancellationtoken.h"

#include "contentsummary.h"

#include "iapi.h"

class Api : public QObject, public IApi
{
    Q_OBJECT

public:
    // Default configuration using Config
    Api(CancellationToken t_cancellationToken);

    Api(QString t_mainApiUrl,
        QStringList t_cacheApiUrls,
        int t_minTimeout,
        int t_maxTimeout,
        CancellationToken t_cancellationToken);

    virtual const QString& getMainApiUrl() const override;
    virtual const QStringList& getApiUrls() const override;

    virtual ContentSummary downloadContentSummary(ApiOperationCallback cb, bool* ok) const override;
    virtual int            downloadPatcherVersion(ApiOperationCallback cb, bool* ok) const override;
    virtual QString        downloadPatcherSecret(ApiOperationCallback cb, bool* ok) const override;
    virtual QStringList    downloadContentUrls(ApiOperationCallback cb, bool* ok) const override;

private:
    QString m_mainApiUrl;
    QStringList m_cacheApiUrls;

    template <class T>
    T downloadInternal(const QString& t_url,
                       T (*t_parser)(QString t_rawData, bool* t_ok),
                       bool* ok = nullptr)
    {
        QString raw = downloadRawDataInternal(t_url);

        bool parsingOk = false;
        T parsed = t_parser(raw, &parsingOk);

        if (ok)
        {
            *ok = parsingOk;
        }

        return parsed;
    }

    QString downloadRawDataInternal(const QString& t_url);

    int m_minTimeout;
    int m_maxTimeout;

    CancellationToken m_cancellationToken;


};
