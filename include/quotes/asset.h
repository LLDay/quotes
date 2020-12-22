#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace quotes {

using RawType = std::string;

struct Quote {
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using ValueType = double;

    TimePoint time;
    ValueType value;
    ValueType relativeDiff;
    ValueType absoluteDiff;
};

class HistoryPoint {
 public:
    using ValueType = Quote::ValueType;
    using TimePoint = Quote::TimePoint;

 public:
    bool operator<(const HistoryPoint & point) const noexcept;

 public:
    ValueType value;
    TimePoint time;
};

class Asset {
 public:
    using ValueType = Quote::ValueType;
    using TimePoint = Quote::TimePoint;
    using HistoryType = std::vector<HistoryPoint>;

 public:
    explicit Asset(const std::string & name) noexcept;

    void updateQuote(TimePoint time, ValueType value) noexcept;

    std::string getName() const noexcept;

    Quote getCurrentQuote() const noexcept;

    HistoryPoint getHistory() const noexcept;

 private:
    std::string mName;
    HistoryType mHistory;
};

}  // namespace quotes
