#include "basedownloadstrategy.h"

#include "downloaderoperator.h"

#include <QMessageBox>

void BaseDownloadStrategy::start(DownloaderOperator* t_operator)
{
    m_operator = t_operator;

    init();
}

void BaseDownloadStrategy::end()
{
    finish();
}

QByteArray BaseDownloadStrategy::data()
{
    return m_data;
}

void BaseDownloadStrategy::proceed()
{
    proceedInternal();
}

void BaseDownloadStrategy::stop()
{
    stopInternal();
}