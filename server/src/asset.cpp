#include <algorithm>

#include "quotes.pb.h"
#include "quotes/asset.h"
#include "quotes/types.h"

namespace quotes {

bool HistoryPoint::operator<(const HistoryPoint & point) const noexcept {
    return time < point.time;
}

Asset::Asset(std::string name) noexcept : mName{std::move(name)} {}

void Asset::add(HistoryPoint point) noexcept {
    auto position = std::lower_bound(mHistory.begin(), mHistory.end(), point);
    mHistory.emplace(position, std::move(point));
}

void Asset::add(proto::HistoryPoint protoPoint) noexcept {
    HistoryPoint point;
    point.time = protoPoint.time();
    point.value = protoPoint.value();
    add(point);
}

void Asset::add(const HistoryType & history) noexcept {
    mHistory.insert(mHistory.end(), history.begin(), history.end());
    std::sort(mHistory.begin(), mHistory.end());
}

std::string Asset::name() const noexcept {
    return mName;
}

proto::Asset Asset::toProto() const noexcept {
    proto::Asset protoAsset;
    protoAsset.set_name(mName);
    for (auto & point : mHistory) {
        auto protoPoint = protoAsset.add_history();
        protoPoint->set_time(point.time);
        protoPoint->set_value(point.value);
    }
    return protoAsset;
}

HistoryType Asset::history() const noexcept {
    return mHistory;
}

Asset Asset::truncate(ValueType size) const noexcept {
    Asset truncated{*this};
    truncated.mHistory.clear();

    auto actualSize = std::min(size, mHistory.size());
    auto fromIt = std::prev(mHistory.end(), actualSize);

    std::copy(fromIt, mHistory.end(), std::back_inserter(truncated.mHistory));

    return truncated;
}

Asset Asset::truncate(TimePoint from, TimePoint to) const noexcept {
    Asset truncated{*this};
    truncated.mHistory.clear();

    HistoryPoint fromPoint;
    HistoryPoint toPoint;

    fromPoint.time = from;
    toPoint.time = to;

    auto fromIt = std::lower_bound(mHistory.begin(), mHistory.end(), fromPoint);
    auto toIt = std::upper_bound(mHistory.begin(), mHistory.end(), toPoint);

    std::copy(fromIt, toIt, std::back_inserter(truncated.mHistory));
    return truncated;
}

}  // namespace quotes
