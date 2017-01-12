/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mockedreply.h"

#include <QTimer>
#include <QtGlobal>

MockedNetworkReply::MockedNetworkReply(QObject* t_parent)
    : QNetworkReply(t_parent)
{
}

void MockedNetworkReply::setContent( const QString& t_conent )
{
    setContent(t_conent.toUtf8());
}

void MockedNetworkReply::setContent( const QByteArray& t_conent )
{
    m_content = t_conent;
    m_contentOffset = 0;

    open(ReadOnly | Unbuffered);
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(t_conent.size()));

    QTimer::singleShot( 500, this, &QNetworkReply::readyRead);
    QTimer::singleShot( 500, this, &QNetworkReply::finished);
    QTimer::singleShot( 500, this, [&]() {
        this->setFinished(true);

    });
}

void MockedNetworkReply::abort()
{
    // NOOP
}

qint64 MockedNetworkReply::bytesAvailable() const
{
    return m_content.size() - m_contentOffset;
}

bool MockedNetworkReply::isSequential() const
{
    return true;
}

qint64 MockedNetworkReply::readData(char* t_data, qint64 t_maxSize)
{
    if (m_contentOffset >= m_content.size())
        return -1;

    qint64 readSize = qMin(t_maxSize, m_content.size() - m_contentOffset);
    memcpy(t_data, m_content.constData() + m_contentOffset, readSize);
    m_contentOffset += readSize;

    return readSize;
}
