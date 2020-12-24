#pragma once

#include <deque>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/shared_ptr.hpp>

namespace quotes {

class HistoryPoint;

using ClientId = int;
using HistoryType = std::deque<HistoryPoint>;
using RawType = std::string;
using Service = boost::shared_ptr<boost::asio::io_service>;
using SizeType = uint32_t;
using TimePoint = uint64_t;
using ValueType = uint64_t;

}  // namespace quotes
