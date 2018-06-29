#pragma once

#include <vector>

#include "downloader.h"

class IDownloaderPool
{
public:
    typedef std::function<bool(const Downloader*)> TDownloaderPredicate;

    virtual std::vector<Downloader*> getDownloaders(TDownloaderPredicate t_predicate) const;
    virtual std::vector<Downloader*> getDownloaders() const = 0;

    virtual void add(Downloader* t_downloader) = 0;
    virtual void remove(Downloader* t_downloader) = 0;

    void append(const std::vector<Downloader*>& t_downloaders);

    std::vector<Downloader*> getFinishedDownloaders() const;
    std::vector<Downloader*> getActiveDownloaders() const;
    std::vector<Downloader*> getStartingDownloaders() const;
    std::vector<Downloader*> getInactiveDownloaders() const;

    size_t poolSize() const;
};
