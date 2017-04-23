/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include <memory>

#include "downloader.h"

class DownloadOperator : public QObject
{
    Q_OBJECT
public:
    explicit DownloadOperator(QObject* parent = 0);
    DownloadOperator
        (QObject* parent, Downloader& downloader);

    bool startDownload();

private:
    std::shared_ptr<Downloader> m_downloader;
};
