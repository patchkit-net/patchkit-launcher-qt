#ifndef DOWNLOADEROPERATOR_H
#define DOWNLOADEROPERATOR_H

#include <vector>
#include <deque>

#include <QVector>

#include "downloader.h"
#include "basedownloadstrategy.h"

#include "urlprovider.h"

class DownloaderOperator : public QObject
{
    Q_OBJECT
public:
    DownloaderOperator(Downloader::TDataSource t_dataSource, const IUrlProvider& t_urlProvider, CancellationToken t_cancellationToken, QObject* parent = nullptr);
    ~DownloaderOperator();

    QByteArray download(BaseDownloadStrategy* t_downloadStrategy = nullptr);

    // Returns all downloaders that have been started and have successfully started downloading
    std::vector<Downloader*> getActiveDownloaders() const;

    // Returns all downloaders that have been started but haven't started downloading yet
    std::vector<Downloader*> getStartingDownloaders() const;

    // Returns all downloaders that haven't been started ( or aren't running )
    std::vector<Downloader*> getInactiveDownloaders() const;

    std::vector<Downloader*> getDownloaders(bool (*t_predicate)(Downloader*) = nullptr) const;

    void stopAll();

signals:
    void downloadProgress(long long t_bytesDownloaded, long long t_totalBytes);

private:
    CancellationToken m_cancellationToken;
    std::vector<Downloader*> m_downloaders;
};

#endif // DOWNLOADEROPERATOR_H
