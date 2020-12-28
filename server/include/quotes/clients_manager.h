#pragma once

#include <map>

#include <boost/thread/mutex.hpp>

#include "quotes/types.h"

namespace quotes {

class ClientsManager {
 public:
    ClientsManager() = default;

    void addClient(ClientId id, SessionPointer session) noexcept;

    bool removeClient(ClientId id) noexcept;

    bool removeClient(SessionPointer session) noexcept;

 private:
    std::map<ClientId, SessionPointer> mClients;
    boost::mutex mMutex;
};

}  // namespace quotes
