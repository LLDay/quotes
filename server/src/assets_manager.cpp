#include "quotes/assets_manager.h"
#include <utility>
#include "quotes/asset.h"

namespace quotes {

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

}  // namespace quotes
