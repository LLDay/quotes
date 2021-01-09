#include "quotes/assets_manager.h"

#include <fstream>
#include <iterator>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "quotes/asset.h"
#include "quotes/assets_manager.h"
#include "quotes/log.h"

namespace quotes {

AssetsManager::AssetsManager(const std::string & path) noexcept
    : mSavePath(path) {
    if (!mSavePath.empty()) {
        std::ifstream inputStream{mSavePath, std::ios::binary};
        if (!inputStream.is_open())
            return;

        boost::archive::binary_iarchive archive{inputStream};
        AssetsManager readAssets;
        archive >> readAssets;
        inputStream.close();

        mAssets = readAssets.mAssets;
        log("Read", mAssets.size(), "assets from", mSavePath);
    }
}

AssetsManager::~AssetsManager() noexcept {
    if (!mSavePath.empty()) {
        std::ofstream outputStream{mSavePath, std::ios::out | std::ios::binary};
        if (!outputStream.is_open())
            return;

        boost::archive::binary_oarchive archive{outputStream};
        archive << *this;
        outputStream.close();
        log("Save", mAssets.size(), "assets to", mSavePath);
    }
}

bool AssetsManager::remove(const std::string & name) noexcept {
    return mAssets.erase(name) != 0;
}

bool AssetsManager::has(const std::string & name) noexcept {
    return mAssets.find(name) != mAssets.end();
}

Asset & AssetsManager::getOrCreate(const std::string & name) noexcept {
    if (!has(name)) {
        log("New asset:", name);
        mAssets.emplace(std::make_pair(name, Asset{name}));
    }

    return mAssets.at(name);
}

std::vector<std::string> AssetsManager::getAllNames() const noexcept {
    std::vector<std::string> allNames;
    for (auto & pair : mAssets)
        allNames.push_back(pair.first);
    return allNames;
}

}  // namespace quotes
