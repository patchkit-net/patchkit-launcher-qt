#ifndef IAPI_H
#define IAPI_H

#include "contentsummary.h"

class IApi
{
public:
    struct ApiOperationStatus
    {
        ApiOperationStatus(int _numberOfTries,
                           QString _previousApiUrl,
                           QString _nextApiUrl,
                           int _currentTimeout,
                           int _nextTimeout)
            : numberOfTries(_numberOfTries)
            , previousApiUrl(_previousApiUrl)
            , nextApiUrl(_nextApiUrl)
            , currentTimeout(_currentTimeout)
            , nextTimeout(_nextTimeout)
        {}

        int numberOfTries;

        QString previousApiUrl;
        QString nextApiUrl;

        int currentTimeout;
        int nextTimeout;
    };

    // Returned bool indicates if the operatino should continue or not
    typedef bool (*ApiOperationCallback)
        (const IApi*, ApiOperationStatus);

    virtual const QString&      getMainApiUrl() const = 0;
    virtual const QStringList&  getApiUrls()    const = 0;

    virtual ContentSummary  downloadContentSummary(ApiOperationCallback cb = nullptr, bool* ok = nullptr)    const = 0;
    virtual int             downloadPatcherVersion(ApiOperationCallback cb = nullptr, bool* ok = nullptr)    const = 0;
    virtual QString         downloadPatcherSecret(ApiOperationCallback cb = nullptr, bool* ok = nullptr)     const = 0;
    virtual QStringList     downloadContentUrls(ApiOperationCallback cb = nullptr, bool* ok = nullptr)       const = 0;
};

/*
 * Example usage:
 * IApi* m_api;
 *
 * bool ok = false;
 * QString secret = m_api->downloadPatcherSecret([](const IApi* api, ApiOperationStatus status)
 * {
 *   // The fact that we're in here means that at least one unsuccessful download occured.
 *   if (status.numberOfTries > 10)
 *   {
 *     // Display Dialog - A Blocking Operation
 *     if (dialogResult == OK)
 *       return true; // Operatino should continue
 *     else
 *       return false; // Finish the operation and try something else
 *   }
 * }, &ok);
 *
 * if (ok)
 * {
 *
 * }
 * else
 * {
 * }
 *
 */

/*
 * Foolhardy example usage:
 * IApi* m_api;
 *
 * QString secret = m_api->downloadPatcherSecret();
 */

#endif // IAPI_H
