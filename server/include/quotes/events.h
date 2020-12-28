#pragma once

#include <boost/enable_shared_from_this.hpp>

#include "quotes/types.h"

namespace quotes {

namespace proto {
class Packet;
}

class Session;

class IEvents : public boost::enable_shared_from_this<IEvents> {
 protected:
    IEvents() = default;

 public:
    virtual ~IEvents() = default;

    virtual void onPacketRead(
        SessionPointer from,
        const proto::Packet & packet) noexcept = 0;

    virtual void onSessionDisconnected(SessionPointer session) noexcept = 0;
};

}  // namespace quotes
