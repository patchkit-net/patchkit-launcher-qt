#ifndef IDOWNLOADSTRATEGY_H
#define IDOWNLOADSTRATEGY_H

#include <QStringList>
#include <QString>
#include <vector>

class DownloaderOperator;

class IResourceUrlProvider
{
public:
    virtual int getVariantCount() const = 0;
    virtual QString getVariant(int index) const = 0;
};

class StringUrlProvider : public IResourceUrlProvider
{
public:
    StringUrlProvider(const QStringList& urls)
    {
        for (QString url : urls)
        {
            m_variants.push_back(url);
        }
    }

    virtual int getVariantCount() const override
    {
        return m_variants.size();
    }

    virtual QString getVariant(int index) const override
    {
        return m_variants.at(index);
    }
private:
    std::vector<QString> m_variants;
};

class StringConcatUrlProvider : public IResourceUrlProvider
{
public:
    StringConcatUrlProvider(const QStringList& bases, const QString& end)
    {
        for (QString b : bases)
        {
            m_variants.push_back(b + "/" + end);
        }
    }

    virtual int getVariantCount() const override
    {
        return m_variants.size();
    }

    virtual QString getVariant(int index) const override
    {
        return m_variants.at(index);
    }

private:
    std::vector<QString> m_variants;
};

class IDownloadStrategy
{
public:
    virtual void init(const DownloaderOperator* t_operator) = 0;
    virtual void finish() = 0;
};

#endif // IDOWNLOADSTRATEGY_H
