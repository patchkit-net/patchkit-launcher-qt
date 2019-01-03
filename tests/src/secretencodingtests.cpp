#include "catch.h"

#include <data/secretencoding.h>

TEST_CASE("Encoding and decoding a value")
{
    QString originalData = "1234567890";

    QByteArray encoded = data::secret::encode(originalData);

    QString decoded = data::secret::decode(encoded);

    REQUIRE(originalData == decoded);
}
