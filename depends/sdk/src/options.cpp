#include "options.h"

bool options::is_args_valid(int argc, const char * const * argv, const boost::program_options::options_description &opt_desc,
                            const boost::program_options::positional_options_description &pos_opt_desc,
                            boost::program_options::variables_map &varmap, std::ostream& cerr,std::ostream& cout, const std::string& info ) {
    using namespace boost::program_options;
    try {
        store(command_line_parser(argc, const_cast<char**>(const_cast<const char**>(argv)))
                      .style(command_line_style::unix_style)
                      .options(opt_desc)
                      .positional(pos_opt_desc)
                      .run(), varmap);
        notify(varmap);
    } catch (const error &err) {
        cerr << "Error parsing command line: " << err.what() << std::endl << std::endl;
        cerr << info << std::endl;
        cerr << opt_desc << std::endl;
        return false;
    }

    if (varmap.count("help") || varmap.empty()) {
        cout << info << std::endl;
        cout << opt_desc << std::endl;
        return false;
    }

    return true;
}
