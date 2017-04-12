#ifndef URLPROVIDER_H
#define URLPROVIDER_H

#include <QStringList>
#include <QString>
#include <vector>

class IUrlProvider
{
public:
    virtual int getVariantCount() const = 0;
    virtual QString getVariant(int index) const = 0;
};

class StringUrlProvider : public IUrlProvider
{
public:
    StringUrlProvider(const QStringList& urls);

    virtual int getVariantCount() const override;

    virtual QString getVariant(int index) const override;
private:
    std::vector<QString> m_variants;
};

// TODO: Reconsider naming of 'bases' and 'end' to match actual URL parts.
class StringConcatUrlProvider : public IUrlProvider
{
public:
    StringConcatUrlProvider(const QStringList& bases, const QString& end);

    virtual int getVariantCount() const override;

    virtual QString getVariant(int index) const override;

private:
    std::vector<QString> m_variants;
};

#endif // URLPROVIDER_H
