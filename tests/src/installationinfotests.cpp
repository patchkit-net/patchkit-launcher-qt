#include "catch.h"

#include "src/data/installationinfo.h"

#include <QBuffer>

TEST_CASE("Saving to device")
{
    QBuffer buffer;

    InstallationInfo installationInfo;
    installationInfo.add("1");
    installationInfo.add("2");

    buffer.open(QIODevice::WriteOnly);
    installationInfo.saveTo(buffer);

    REQUIRE(buffer.data().toStdString() == "1\n2\n");
}

TEST_CASE("Reading from device")
{
    QBuffer buffer;

    buffer.setData("1\n2\n");
    buffer.open(QIODevice::ReadOnly);
    auto installationInfo = InstallationInfo::from(buffer);
    auto files = installationInfo.installedFiles();

    REQUIRE(files.contains("1"));
    REQUIRE(files.contains("2"));
}
