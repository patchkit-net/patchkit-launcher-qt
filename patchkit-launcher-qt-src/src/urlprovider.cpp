#include "urlprovider.h"

#include "config.h"

StringUrlProvider::StringUrlProvider(const QStringList& urls)
{
    for (QString url : urls)
    {
        m_variants.push_back(url);
    }
}

int StringUrlProvider::getVariantCount() const
{
    return (int) m_variants.size();
}

QString StringUrlProvider::getVariant(int index) const
{
    auto var = m_variants.at(index);

    if (m_countryCode != QString())
    {
        var += "?country=" + m_countryCode;
    }

    if (m_identifierVersion != QString())
    {
        var += "?caller=launcher:" + m_identifierVersion;
    }

    return var;
}

void StringUrlProvider::setCountryCode(const QString& t_countryCode)
{
    m_countryCode = t_countryCode;
}

void StringUrlProvider::setIdentifier(const QString& t_version)
{
    m_identifierVersion = t_version;
}

StringConcatUrlProvider::StringConcatUrlProvider(const QStringList& hosts, const QString& path)
    : StringUrlProvider(QStringList())
{
    for (QString host : hosts)
    {
        m_variants.push_back(host + "/" + path);
    }
}
