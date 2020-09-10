#include "ntp.h"

#include <limits>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

using namespace datetime;

ntp_t datetime::unpack(uint64_t value) {
    return {static_cast<uint32_t>(value >> 32), static_cast<uint32_t>(value & std::numeric_limits<uint32_t>::max())};
}

uint64_t datetime::pack(const ntp_t &value) {
    return static_cast<uint64_t>(value.seconds) << 32 | static_cast<uint64_t>(value.fraction);
}

ntp_t datetime::from_iso8601(const std::string &str) {

    using boost::posix_time::ptime;
    using boost::posix_time::time_duration;
    using boost::gregorian::date;

    boost::local_time::local_time_input_facet *input_facet = new boost::local_time::local_time_input_facet("%Y-%m-%d %H:%M:%S%F");
    std::stringstream ss;
    ss.imbue(std::locale(ss.getloc(), input_facet)); //input_facet is deleted by STL; refer ISO C++ Spec ch.22.3.1.6 and §22.3.1.1.2
    ss.exceptions(std::ios_base::failbit);
    ss.str(str);

    boost::local_time::local_date_time ldt(boost::local_time::not_a_date_time);
    ss >> ldt;

    auto time = ldt.utc_time();

    static const ptime ntp_epoch_start(date(1900, boost::gregorian::Jan, 1));
    time_duration elapsed_since_ntp_epoch = time - ntp_epoch_start;
    auto ntp_seconds = static_cast<uint32_t>(elapsed_since_ntp_epoch.total_seconds());
    auto ntp_fraction =  static_cast<uint32_t>(elapsed_since_ntp_epoch.fractional_seconds() * ntp_t::fractions_per_second / time_duration::ticks_per_second());
    return {ntp_seconds, ntp_fraction};
}
