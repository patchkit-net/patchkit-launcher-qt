/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "remotepatcherdata.h"

#include "logger.h"
#include "config.h"
#include "customexceptions.h"
#include "chunkeddownloader.h"
#include "contentsummary.h"

#include "api.h"

RemotePatcherData::RemotePatcherData(IApi& t_api, QNetworkAccessManager* t_networkAccessManager)
    : m_networkAccessManager(t_networkAccessManager)
    , m_api(t_api)
{
}

int RemotePatcherData::getVersion(const Data& t_data, CancellationToken t_cancellationToken) const
{
    return m_api.getLatestVersionId(t_data.patcherSecret(), t_cancellationToken);
}

QString RemotePatcherData::getPatcherSecret(const QString& t_applicationSecret, CancellationToken t_cancellationToken) const
{
    QString patcherSecret = m_api.getPatcherSecret(t_applicationSecret, t_cancellationToken);

    if (patcherSecret.isNull() || patcherSecret.isEmpty())
    {
        qInfo() << "No patcher secret found, attempting to download default patcher secret.";
        patcherSecret = m_api.getDefaultPatcherSecret(t_cancellationToken);
    }

    return patcherSecret;
}

void RemotePatcherData::download(QIODevice& t_dataTarget, const Data& t_data, int t_version, CancellationToken t_cancellationToken)
{
    qInfo("Downloading patcher %d version", t_version);

    QStringList contentUrls = m_api.getContentUrls(t_data.patcherSecret(), t_version, t_cancellationToken);

    QString patcherSecret = t_data.patcherSecret();

    ContentSummary summary;

    summary = m_api.getContentSummary(patcherSecret, t_version, t_cancellationToken);

    if (summary.isValid())
    {
        ChunkedDownloader downloader(
                    m_networkAccessManager,
                    summary,
                    HashingStrategy::xxHash,
                    t_cancellationToken,
                    m_api);

        connect(&downloader, &ChunkedDownloader::downloadProgress, this, &RemotePatcherData::downloadProgressChanged);

        QByteArray data = downloader.downloadFile(contentUrls);

        saveData(data, t_dataTarget);

        return;
    }
    else
    {
        qCritical("INVALID CONTENT SUMMARY");
    }

    throw std::runtime_error("Unable to download patcher version - " + std::to_string(t_version));
}

bool RemotePatcherData::saveData(QByteArray& t_data, QIODevice& t_dataTarget)
{
    if (!t_dataTarget.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open data target for writing.");
        return false;
    }

    qInfo("Saving downloaded data.");

    t_dataTarget.write(t_data);

    t_dataTarget.close();

    return true;
}
