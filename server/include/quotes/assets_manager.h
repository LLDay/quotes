#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include "quotes/asset.h"
#include "quotes/types.h"

namespace quotes {

class AssetsManager {
 private:
    friend class boost::serialization::access;

    template <class Archive>
    void save(Archive & archive, const unsigned int /*version*/) const {
        archive << static_cast<SizeType>(mAssets.size());

        for (auto & nameAsset : mAssets) {
            auto history = nameAsset.second.history();

            archive << nameAsset.second.name();
            archive << static_cast<SizeType>(history.size());

            for (auto & moment : history) {
                archive << moment.time;
                archive << moment.value;
            }
        }
    }

    template <class Archive>
    void load(Archive & archive, const unsigned int /*version*/) {
        SizeType assetsNumber;
        archive >> assetsNumber;

        for (SizeType a = 0; a < assetsNumber; ++a) {
            std::string assetName;
            SizeType pointsNumber;
            archive >> assetName;
            archive >> pointsNumber;

            Asset asset{assetName};
            HistoryType history;
            for (SizeType p = 0; p < pointsNumber; ++p) {
                HistoryPoint point;
                archive >> point.time;
                archive >> point.value;
                history.emplace_back(std::move(point));
            }

            asset.add(history);
            mAssets.emplace(assetName, std::move(asset));
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();

 public:
    AssetsManager() = default;

    explicit AssetsManager(const std::string & path) noexcept;

    ~AssetsManager() noexcept;

    bool remove(const std::string & name) noexcept;

    bool has(const std::string & name) noexcept;

    Asset & getOrCreate(const std::string & name) noexcept;

    std::vector<std::string> getAllNames() const noexcept;

 private:
    size_t totalHistoryPoints() const noexcept;

 private:
    std::map<std::string, Asset> mAssets;
    std::string mSavePath;
};

}  // namespace quotes
