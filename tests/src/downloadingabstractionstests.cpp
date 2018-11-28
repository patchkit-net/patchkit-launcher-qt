#include "catch.h"

#include "src/remote/downloading/downloadingabstractions.h"
#include "src/cancellation/cancellationtokensource.h"
#include "src/data/downloadrange.h"
#include "mockednam.h"
#include "mockedreply.h"

#include <QBuffer>

TEST_CASE("Reply buffering")
{
    using namespace downloading::abstractions;

    QByteArray data = "1234567890";
    MockedNetworkReply reply(1, data, 200);
    reply.launch();

    QBuffer target;
    target.open(QIODevice::WriteOnly);

    CancellationTokenSource cancellation;

    bufferReply(&reply, target, 200, cancellation);

    REQUIRE(target.data().toStdString() == data.toStdString());
}

TEST_CASE("Basic download")
{
    using namespace downloading::abstractions;

    MockedNAM nam;
    QByteArray data = "1234567890";

    QBuffer target;
    target.open(QIODevice::WriteOnly);

    nam.push("host.com/123", data, 0);

    CancellationTokenSource cancellation;

    bool result = tryDownload(nam, "host.com/123", target, 0, cancellation);

    CHECK(result == true);
    REQUIRE(target.data().toStdString() == data.toStdString());
}

TEST_CASE("Ranged download")
{
    using namespace downloading::abstractions;

    MockedNAM nam;
    QByteArray data = "1234567890";

    QBuffer target;
    target.open(QIODevice::WriteOnly);

    nam.push("host.com/123", data, 0);

    CancellationTokenSource cancellation;
    data::DownloadRange range(1);

    bool result = tryRangedDownload(nam, "host.com/123",
        range, target, 100, cancellation);

    CHECK(result == true);
    REQUIRE(target.data().toStdString() == "234567890");
}
