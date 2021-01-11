#include "quotes/utils.h"
#include <bits/stdint-uintn.h>

#include <boost/date_time/gregorian/greg_date.hpp>
#include <string>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace quotes {

std::string timeToString(uint64_t microseconds) noexcept {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    boost::posix_time::ptime maxTime(boost::gregorian::date(9999, 1, 1));
    auto ptime = epoch + boost::posix_time::microseconds(microseconds);

    uint64_t highestBit = 1;
    highestBit = highestBit << 63;

    if (ptime >= maxTime || microseconds & highestBit)
        return "a time after 9999-Jan-01";

    return boost::posix_time::to_simple_string(ptime);
}

}  // namespace quotes
