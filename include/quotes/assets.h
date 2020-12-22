#pragma once

#include <map>
#include <string>

#include "quotes/asset.h"

namespace quotes {

class Assets {
 public:
    Assets() = default;

    void addAsset(std::string asset) noexcept;

    bool removeAsset(const std::string & asset) noexcept;

    Asset & getAsset() noexcept;

 private:
    std::map<std::string, Asset> mAssets;
};

}  // namespace quotes
