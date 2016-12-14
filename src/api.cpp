/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "downloader.h"
#include "timeoutexception.h"
#include "config.h"

#include "contentsummary.h"

Api::Api(QObject* parent) : QObject(parent)
{
}

QString Api::downloadString(const QString& t_resourceUrl, CancellationToken t_cancellationToken) const
{
    QStringList cacheApiUrls = Config::cacheApiUrls;
    return downloadString(t_resourceUrl, cacheApiUrls, false, t_cancellationToken);
}

QJsonDocument Api::downloadContentSummary(const QString& t_resourceUrl, CancellationToken t_cancellationToken) const
{
	QJsonDocument document;
	do
	{
		t_cancellationToken.throwIfCancelled();

		QString raw = downloadString(t_resourceUrl, t_cancellationToken);

		document = QJsonDocument::fromJson(raw.toUtf8());
	} 
	while (document.isNull() || document.isEmpty());

    return document;
}

QString Api::downloadString(const QString& t_resourceUrl, QStringList& t_cacheApiUrls, bool t_extendedTimeout, CancellationToken t_cancellationToken) const
{
    QString result;
    int statusCode;

    int timeout = t_extendedTimeout ? Config::maxConnectionTimeoutMsec : Config::minConnectionTimeoutMsec;

    if (downloadStringFromServer(Config::mainApiUrl + "/" + t_resourceUrl, timeout, result, statusCode, t_cancellationToken))
    {
        if (!isVaild(statusCode))
        {
            throw std::runtime_error("API response error. Status code - " + std::to_string(statusCode));
        }

        return result;
    }

    for (int i = 0; i < t_cacheApiUrls.length(); i++)
    {
        if (downloadStringFromServer(t_cacheApiUrls[i] + "/" + t_resourceUrl, timeout, result, statusCode, t_cancellationToken))
        {
            if (isVaild(statusCode))
            {
                return result;
            }

            t_cacheApiUrls.removeAt(i);
            i--;
        }
    }

    if (t_extendedTimeout)
    {
        throw std::runtime_error("API connection error.");
    }

    return downloadString(t_resourceUrl, t_cacheApiUrls, true, t_cancellationToken);
}

bool Api::isVaild(int t_statusCode) const
{
    return t_statusCode == 200;
}

bool Api::downloadStringFromServer(const QString& t_url, int t_timeout, QString& t_result, int& t_statusCode, CancellationToken t_cancellationToken) const
{
    try
    {
        Downloader downloader;
        t_result = downloader.downloadString(t_url, t_timeout, t_statusCode, t_cancellationToken);

        if (t_statusCode == 500)
        {
            return false;
        }

        return true;
    }
    catch (TimeoutException&)
    {
        return false;
    }
}
