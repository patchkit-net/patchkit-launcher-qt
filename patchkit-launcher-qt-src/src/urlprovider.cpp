#include "urlprovider.h"

StringUrlProvider::StringUrlProvider(const QStringList& urls)
{
    for (QString url : urls)
    {
        m_variants.push_back(url);
    }
}

int StringUrlProvider::getVariantCount() const
{
    return m_variants.size();
}

QString StringUrlProvider::getVariant(int index) const
{
    return m_variants.at(index);
}

StringConcatUrlProvider::StringConcatUrlProvider(const QStringList& hosts, const QString& path)
{
    for (QString b : hosts)
    {
        m_variants.push_back(b + "/" + path);
    }
}

int StringConcatUrlProvider::getVariantCount() const
{
    return m_variants.size();
}

QString StringConcatUrlProvider::getVariant(int index) const
{
    return m_variants.at(index);
}
