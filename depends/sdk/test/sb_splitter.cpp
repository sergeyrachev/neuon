#include "extraction.h"
#include "version.h"
#include "birthday.h"
#include "options.h"
#include "sighandler.h"

#include "spdlog/spdlog.h"

#include <string>
#include <iostream>
#include <csignal>

#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
    std::string media_filename;

    std::cout << std::string(argv[0]) << " :: " << mementor::version << " :: " << mementor::birthday << std::endl;
    namespace po = boost::program_options;
    po::options_description opt_desc("Options");
    opt_desc.add_options()
                ("help,h", "Produce this message")
                ("input-media,i", po::value(&media_filename)->required(), "Input media clip to detect A/V sync")
                ;

    po::positional_options_description pos_opt_desc;
    po::variables_map varmap;
    if (!options::is_args_valid(argc, argv, opt_desc, pos_opt_desc, varmap, std::cerr, std::cout)) {
        if (varmap.count("help") || varmap.empty()) {
            return 0;
        }
        return 1;
    }

    extraction_t splitter(media_filename, 720, 44100,
        [](ffmpeg::frame_ptr au, const std::chrono::microseconds &) {
            SPDLOG_INFO("Video: {}", au->pts);
        },
        [](ffmpeg::frame_ptr au, const std::chrono::microseconds &) {
            SPDLOG_INFO("Audio: {}", au->pts);
        });

    threads::interruption_t interruption;
    posix::sighandler_t<SIGINT>::assign([&interruption](int signal) { interruption.interrupt(); });
    posix::sighandler_t<SIGTERM>::assign([&interruption](int signal) { interruption.interrupt(); });

    splitter.src->run(interruption);


}
