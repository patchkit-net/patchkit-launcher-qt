/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>
#include <QApplication>

#include "src/chunkeddownloader.h"
#include "src/timeoutexception.h"
#include "src/contentsummary.h"

#include "mockednam.h"
#include "custommacros.h"

SCENARIO("Testing chunk verification.")
{
    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);
    MockedNAM nam;

    GIVEN("A some data with a content summary")
    {
        int chunkSize = 2;
        QByteArray data = "veryrandomstringofdata";
        ContentSummary contentSummary = ContentSummary::fromData(data, chunkSize, &HashingStrategy::xxHash);

        ChunkedDownloader chunkedDownloader(&nam, contentSummary, &HashingStrategy::xxHash, token);

        THEN("Given valid data, verification should complete without issues.")
        {
            QVector<QByteArray> chunks = chunkedDownloader.processChunks(data);
            QVector<bool> validChunkMap = chunkedDownloader.validateAllChunks(chunks);

            REQUIRE(validChunkMap.contains(false) == false);
        }

        THEN("Given incomplete data, verification should complete with an incomplete map.")
        {
            QByteArray incompleteData = data.mid(0, chunkSize * 3);

            QVector<QByteArray> chunks = chunkedDownloader.processChunks(incompleteData);
            QVector<bool> validChunkMap = chunkedDownloader.validateAllChunks(chunks);

            REQUIRE(validChunkMap.contains(false) == false);
            REQUIRE(validChunkMap.size() != contentSummary.getChunksCount());
        }

        THEN("Given complete but corrupted data, verification should complete with a fully invalid map.")
        {
            QByteArray insanelyCorruptedData = data;
            for (int i = 0; i < insanelyCorruptedData.size(); i++)
            {
                insanelyCorruptedData[i] = insanelyCorruptedData[i] + 1;
            }

            QVector<QByteArray> chunks = chunkedDownloader.processChunks(insanelyCorruptedData);
            QVector<bool> validChunkMap = chunkedDownloader.validateAllChunks(chunks);

            REQUIRE(validChunkMap.contains(true) == false);
            REQUIRE(validChunkMap.size() == contentSummary.getChunksCount());
        }
    }
}
