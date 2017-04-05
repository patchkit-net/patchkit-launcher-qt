#ifndef IAPI_H
#define IAPI_H

class ContentSummary;
class QString;
class QStringList;

class IApi
{
public:
    virtual ContentSummary  downloadContentSummary(const QString& t_resourceUrl) = 0;
    virtual QString         downloadPatcherSecret(const QString& t_resourceUrl) = 0;
    virtual int             downloadPatcherVersion(const QString& t_resourceUrl) = 0;
    virtual QStringList     downloadContentUrls(const QString& t_resourceUrl) = 0;
};

#endif // IAPI_H
