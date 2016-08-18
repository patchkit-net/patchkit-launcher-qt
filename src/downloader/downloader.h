/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once
#include <QtNetwork>
#include <memory>
#include <QtConcurrent/QtConcurrent>
#include <launcherlog.h>

class Downloader : public QObject
{
    Q_OBJECT

protected:
    ~Downloader()
    {
    }

    Downloader(const QString& t_urlPath) :
        m_isCancelled(false),
        m_urlPath(t_urlPath)
    {
    }

public:

    void cancel()
    {
        m_isCancelled = true;
        emit cancelled();
    }

signals:
    void cancelled();

public slots:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);

protected:
    void start()
    {
        QtConcurrent::run(this, &Downloader::download);
    }

    virtual void processReply(std::shared_ptr<QNetworkReply> t_reply) = 0;

    bool m_isCancelled;

private:
    void download()
    {

    }

    void getNetworkReply(const QString& t_urlPath, std::shared_ptr<QNetworkAccessManager>& t_accessManager, std::shared_ptr<QNetworkReply>& t_reply) const
    {
        logInfo("Getting network reply.");

        QUrl url(t_urlPath);
        t_accessManager = std::make_shared<QNetworkAccessManager>();
        t_reply = std::shared_ptr<QNetworkReply>(t_accessManager.get()->get(QNetworkRequest(url)));

        waitForNetworkReply(t_reply);

        validateNetworkReply(t_reply);
    }

    void waitForNetworkReply(std::shared_ptr<QNetworkReply>& t_reply) const
    {
        logInfo("Waiting for network reply to be ready.");

        QEventLoop readyReadLoop;
        connect(t_reply.get(), &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);

        connect(this, &Downloader::cancelled, &readyReadLoop, &QEventLoop::quit);

        QTimer timeoutTimer;

        connect(&timeoutTimer, &QTimer::timeout, &readyReadLoop, &QEventLoop::quit);

        timeoutTimer.setInterval(downloadTimeoutInSeconds * 1000);
        timeoutTimer.setSingleShot(true);
        timeoutTimer.start();

        readyReadLoop.exec();

        if (m_isCancelled)
        {
            throw LauncherCancelledException();
        }

        if (!timeoutTimer.isActive())
        {
            throw LauncherException("Request timeout");
        }
    }

    QString m_urlPath;
};

class FileDownloader : public Downloader
{
public:
    virtual ~FileDownloader()
    {
    }

    FileDownloader(const QString& t_filePath, const QString& t_urlPath):
        Downloader(t_urlPath),
        m_filePath(t_filePath)
    {
    }

protected:
    void processReply(std::shared_ptr<QNetworkReply> t_reply) override;

private:
    QString m_filePath;
    
};