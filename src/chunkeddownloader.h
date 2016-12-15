/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CHUNKEDDOWNLOADER_H
#define CHUNKEDDOWNLOADER_H

#include <QObject>

#include "cancellationtoken.h"

class ContentSummary;

struct Chunk
{
	char* bytes;
	size_t size;
};

class ChunkedDownloader : public QObject
{
	Q_OBJECT

	const long long CHUNK_SIZE = 524288;

public:
	ChunkedDownloader(const ContentSummary& t_contentSummary);

    void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const;

signals:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private slots:
	void watchDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:

	// bool acceptChunk()

	const ContentSummary& m_contentSummary;
    
    int m_maxNumberOfAttepmts;

	std::vector<Chunk> m_chunks;
};


#endif // CHUNKEDDOWNLOADER_H
