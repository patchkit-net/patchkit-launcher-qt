#include "catch.h"

#include <QtNetwork>

class MyNAM : public QNetworkAccessManager
{
    virtual QNetworkReply* get(const QNetworkRequest &request) override
    {

    }
};

SCENARIO("RemotePatcherData with mocked api and downloaders.")
{
    GIVEN("some mocked api.")
    {
        THEN("should 'download' expected data.")
        {

        }
    }
}
