#include "quotes/assets_manager.h"

#include <fstream>
#include <iterator>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "quotes/asset.h"

namespace quotes {

AssetsManager::AssetsManager(std::string path) {
    if (!mSavePath.empty()) {
        std::ifstream inputStream{path, std::ios::binary};
        if (!inputStream.is_open())
            return;

        boost::archive::binary_iarchive archive{inputStream};
        AssetsManager readAssets;
        archive >> readAssets;
        inputStream.close();

        (*this) = readAssets;
        mSavePath = path;
    }
}

AssetsManager::~AssetsManager() noexcept {
    if (!mSavePath.empty()) {
        std::ofstream outputStream{
            mSavePath, std::ofstream::out | std::ofstream::binary};
        if (!outputStream.is_open())
            return;

        boost::archive::binary_oarchive archive{outputStream};
        archive << *this;
        outputStream.close();
    }
}

bool AssetsManager::remove(const std::string & name) noexcept {
    return mAssets.erase(name) != 0;
}

bool AssetsManager::has(const std::string & name) noexcept {
    return mAssets.find(name) != mAssets.end();
}

Asset & AssetsManager::getOrCreate(const std::string & name) noexcept {
    if (!has(name))
        mAssets.emplace(std::make_pair(name, Asset{name}));

    return mAssets.at(name);
}

std::vector<std::string> AssetsManager::getAllNames() const noexcept {
    std::vector<std::string> allNames;
    for (auto & pair : mAssets)
        allNames.push_back(pair.first);
    return allNames;
}

}  // namespace quotes
