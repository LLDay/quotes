#include "quotes/asset.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iterator>
#include <ostream>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include "quotes.pb.h"
#include "quotes/log.h"
#include "quotes/types.h"

namespace quotes {

bool HistoryPoint::operator<(const HistoryPoint & point) const noexcept {
    return time < point.time;
}

bool HistoryPoint::operator==(const HistoryPoint & point) const noexcept {
    return time == point.time;
}

std::ostream & operator<<(
    std::ostream & ostream,
    const HistoryPoint & point) noexcept {
    static auto precision = 6;
    static auto factor = std::pow(10, precision);

    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    auto ptime = epoch + boost::posix_time::microseconds(point.time);

    auto timeString = boost::posix_time::to_simple_string(ptime);
    ostream << "time " << timeString << " value "
            << std::setprecision(precision) << point.value / factor;

    return ostream;
}

Asset::Asset(std::string name) noexcept : mName{std::move(name)} {}

void Asset::add(HistoryPoint point) noexcept {
    auto position = std::lower_bound(mHistory.begin(), mHistory.end(), point);
    if (position != mHistory.end() && position->time == point.time)
        position->value = point.value;
    else
        mHistory.emplace(position, std::move(point));
}

void Asset::add(proto::HistoryPoint protoPoint) noexcept {
    HistoryPoint point;
    point.time = protoPoint.time();
    point.value = protoPoint.value();
    log("Add point:", point);
    add(point);
}

void Asset::add(const HistoryType & history) noexcept {
    auto sortedHistory = history;
    std::sort(sortedHistory.begin(), sortedHistory.end());

    auto from = std::unique(sortedHistory.begin(), sortedHistory.end());
    sortedHistory.erase(from, sortedHistory.end());

    auto thisHistory = std::move(mHistory);
    mHistory.clear();

    std::merge(
        sortedHistory.begin(), sortedHistory.end(), thisHistory.begin(),
        thisHistory.end(), std::back_inserter(mHistory));
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

void Asset::truncate(ValueType size) noexcept {
    auto history = std::move(mHistory);
    mHistory.clear();

    auto actualSize = std::min(size, history.size());
    auto fromIt = std::prev(history.end(), actualSize);

    std::copy(fromIt, history.end(), std::back_inserter(mHistory));
}

void Asset::truncate(TimePoint from, TimePoint to) noexcept {
    auto history = std::move(mHistory);
    mHistory.clear();

    HistoryPoint fromPoint;
    HistoryPoint toPoint;

    fromPoint.time = from;
    toPoint.time = to;

    auto fromIt = std::lower_bound(history.begin(), history.end(), fromPoint);
    auto toIt = std::upper_bound(history.begin(), history.end(), toPoint);

    std::copy(fromIt, toIt, std::back_inserter(mHistory));
}

}  // namespace quotes
