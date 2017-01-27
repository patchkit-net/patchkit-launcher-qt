/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include "src/remotepatcherdata.h"
#include "src/contentsummary.h"

#include "mockednam.h"

namespace RemotePatcherDataTests
{
const QByteArray data = "ABCDEFGHIJ";

class MockedApi : public IApi
{
public:
    MockedApi()
    {
        ContentSummary summary(1, 0, "none", "none", "xxHash",
        {
            HashingStrategy::xxHash(data.mid(0, 1)),
            HashingStrategy::xxHash(data.mid(1, 1)),
            HashingStrategy::xxHash(data.mid(2, 1)),
            HashingStrategy::xxHash(data.mid(3, 1)),
            HashingStrategy::xxHash(data.mid(4, 1)),
            HashingStrategy::xxHash(data.mid(5, 1)),
            HashingStrategy::xxHash(data.mid(6, 1)),
            HashingStrategy::xxHash(data.mid(7, 1)),
            HashingStrategy::xxHash(data.mid(8, 1)),
            HashingStrategy::xxHash(data.mid(9, 1))
        },
        {});

        m_contentSummary = summary.toJson();
    }

    virtual QString downloadString(const QString& t_resourceUrl, CancellationToken /*t_cancellationToken*/) const
    {
        if (t_resourceUrl.endsWith("content_urls")) // Content urls
        {
            return "[{\"url\" : \"link\"}]";
        }
        else if (t_resourceUrl.endsWith("latest/id")) // Version
        {
            return "{\"id\" : 1}";
        }
        else // Patcher secret
        {
            return "{\"patcher_secret\" : \"1\"}";
        }
    }

    virtual QJsonDocument downloadContentSummary(const QString& /*t_resourceUrl*/, CancellationToken /*t_cancellationToken*/) const
    {
        return m_contentSummary;
    }

private:
    QJsonDocument m_contentSummary;
};

struct MockedData : Data
{
    virtual QString patcherSecret() const override
    {
        return "123";
    }

    virtual QString applicationSecret() const override
    {
        return "321";
    }

    virtual QByteArray encodedApplicationSecret() const override
    {
        return "123321";
    }
};

}

SCENARIO("Testing remote patcher data with an appropriate content summary.")
{
    using namespace RemotePatcherDataTests;

    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);

    GIVEN("A mocked NAM replying to 'link' with specified data and a mocked API.")
    {
        MockedNAM nam;
        nam.push("link", data, 10);

        MockedApi api;

        RemotePatcherData remotePatcherData((IApi&)api, &nam);

        QBuffer dataTargetBuffer;

        MockedData mockedData;

        remotePatcherData.download(dataTargetBuffer, (Data&) mockedData, 0, token);

        REQUIRE(dataTargetBuffer.data().toStdString() == data.toStdString());
    }
}
