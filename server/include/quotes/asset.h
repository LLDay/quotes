#pragma once

#include "quotes.pb.h"
#include "quotes/types.h"

namespace quotes {

class HistoryPoint {
 public:
    bool operator<(const HistoryPoint & point) const noexcept;

 public:
    ValueType value;
    TimePoint time;
};

class Asset {
 public:
    explicit Asset(std::string name) noexcept;

    void add(HistoryPoint point) noexcept;

    void add(proto::HistoryPoint protoPoint) noexcept;

    void add(const HistoryType & history) noexcept;

    std::string name() const noexcept;

    proto::Asset toProto() const noexcept;

    HistoryType history() const noexcept;

    Asset truncate(ValueType size) const noexcept;

    Asset truncate(TimePoint from, TimePoint to) const noexcept;

 private:
    std::string mName;
    HistoryType mHistory;
};

}  // namespace quotes
