#include "quotes/assets_manager.h"

#include <optional>

namespace quotes {

void AssetsManager::addAsset(const std::string & name) noexcept {
    mAssets.emplace(std::make_pair(name, Asset{name}));
}

bool AssetsManager::hasAsset(const std::string & name) noexcept {
    return mAssets.find(name) != mAssets.end();
}

bool AssetsManager::removeAsset(const std::string & name) noexcept {
    return mAssets.erase(name) != 0;
}

std::optional<Asset> AssetsManager::getAsset(
    const std::string & name) noexcept {
    auto iterator = mAssets.find(name);
    if (iterator == mAssets.end())
        return std::nullopt;
    return iterator->second;
}

}  // namespace quotes
