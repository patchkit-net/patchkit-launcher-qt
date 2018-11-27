#pragma once

#include "data/data.h"
#include "data/contentsummary.h"
#include "remote/api/api.h"
#include "remote/downloading/downloadingabstractions.h"
#include "remote/downloading/chunked/chunkedbuffer.h"
#include "customexceptions.h"

#include <QNetworkAccessManager>
#include <QIODevice>
#include <QObject>

namespace downloading
{
namespace chunked
{

class Downloader : public QObject
{
    Q_OBJECT
public:
    CUSTOM_RUNTIME_ERROR(InvalidTarget)

    Downloader(
            const QString& appSecret, int versionId,
            const ContentSummary& contentSummary, QIODevice& target);

    bool downloadChunked(
            const Api& api, QNetworkAccessManager& nam,
            CancellationToken cancellationToken);
signals:
    void onProgress(long long downloaded, long long total);

private slots:
    void progressRelay(qint64 bytes);

private:
    const QString& m_appSecret;
    int m_versionId;

    const ContentSummary& m_contentSummary;
    QIODevice& m_target;
};

}
}
