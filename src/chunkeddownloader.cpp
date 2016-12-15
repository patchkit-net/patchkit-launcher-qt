/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QtNetwork>

#include "chunkeddownloader.h"

#include "timeoutexception.h"
#include "contentsummary.h"
#include "downloader.h"
#include "logger.h"

ChunkedDownloader::ChunkedDownloader(const ContentSummary& t_contentSummary)
	: m_contentSummary(t_contentSummary)
{
}

void ChunkedDownloader::downloadFile(const QString & t_urlPath, const QString & t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const
{
	Downloader downloader;

	connect(&downloader, &Downloader::downloadProgressChanged, this, &ChunkedDownloader::downloadProgressChanged);
	//connect(&downloader, &Downloader::downloadProgressChanged, this, &ChunkedDownloader::watchDownloadProgress);

	downloader.downloadFile(t_urlPath, t_filePath, t_requestTimeoutMsec, t_cancellationToken);

}

void ChunkedDownloader::watchDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*> (sender());

	if (reply == nullptr)
		throw std::runtime_error("Unexpected error during chunked file download.");

	// This is where the magic happens

	// Process the read bytes, hash them and do stuff
}
