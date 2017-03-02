#ifndef APIOPERATOR_H
#define APIOPERATOR_H

#include <QJsonDocument>

#include "iapi.h"
#include "contentsummary.h"

class ApiOperator
{
public:
    ApiOperator(const IApi& t_api);

    // Operations
    ContentSummary downloadContentSummary();
    int downloadVersion();
    QString downloadPatcherSecret();


private:

    const IApi& m_api;
};

#endif // APIOPERATOR_H
