#pragma once

#include <deque>
#include <string>

#include <boost/asio/io_service.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace quotes {

class HistoryPoint;
class Session;
class Server;
class IEvents;

using ClientId = int;
using HistoryType = std::deque<HistoryPoint>;
using RawType = std::string;
using Service = boost::shared_ptr<boost::asio::io_service>;
using SizeType = uint32_t;
using TimePoint = uint64_t;
using ValueType = uint64_t;
using ServerPointer = boost::shared_ptr<Server>;
using EventPointer = boost::weak_ptr<IEvents>;
using SessionPointer = boost::shared_ptr<Session>;

}  // namespace quotes
