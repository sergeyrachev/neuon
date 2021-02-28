#include "demo.h"

#include <locale>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

common::demo_t::demo_t(std::string birthday, uint32_t days) {

    boost::posix_time::ptime the_day_of_birth;
    std::istringstream iss(birthday);
    iss.imbue(std::locale(std::locale::classic(), new boost::posix_time::time_input_facet("%Y-%m-%dT%H:%M:%SZ")));
    iss >> the_day_of_birth;

    boost::posix_time::ptime the_today = boost::posix_time::second_clock::universal_time();
    boost::gregorian::date_duration age = the_today.date() - the_day_of_birth.date();

    has_expired = age.days() > days;
}
