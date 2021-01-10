#include "quotes/utils.h"

#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace quotes {

std::string timeToString(uint64_t microseconds) noexcept {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    auto ptime = epoch + boost::posix_time::microseconds(microseconds);

    return boost::posix_time::to_simple_string(ptime);
}

}  // namespace quotes
