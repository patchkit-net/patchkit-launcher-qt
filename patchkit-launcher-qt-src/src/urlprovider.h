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

class StringConcatUrlProvider : public IUrlProvider
{
public:
    StringConcatUrlProvider(const QStringList& hosts, const QString& path);

    virtual int getVariantCount() const override;

    virtual QString getVariant(int index) const override;

private:
    std::vector<QString> m_variants;
};
