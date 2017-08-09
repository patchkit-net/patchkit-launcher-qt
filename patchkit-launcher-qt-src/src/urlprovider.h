/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QStringList>
#include <QString>
#include <vector>

class IUrlProvider
{
public:
    virtual int getVariantCount() const = 0;
    virtual QString getVariant(int index) const = 0;

    virtual void setCountryCode(const QString& t_countryCode) = 0;
};

class StringUrlProvider : public IUrlProvider
{
public:
    StringUrlProvider(const QStringList& urls);

    virtual int getVariantCount() const override;
    virtual QString getVariant(int index) const override;

    virtual void setCountryCode(const QString& t_countryCode) override;

protected:
    std::vector<QString> m_variants;

private:
    QString m_countryCode;
};

class StringConcatUrlProvider : public StringUrlProvider
{
public:
    StringConcatUrlProvider(const QStringList& hosts, const QString& path);
};
