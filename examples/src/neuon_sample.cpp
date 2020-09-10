#include "version.h"
#include "birthday.h"
#include "options.h"
#include "interruption.h"
#include "sighandler.h"
#include "extraction.h"

#include "neuon/neuon_c.h"

#include <chrono>
#include <iostream>
#include <iomanip>

struct statistic_t {
    uint64_t nosync_samples{};
    uint64_t sync_samples{};
};

void print_result(neuon_user_data_t* user_data, const neuon_outcome_t *result) {
    static uint64_t counter = 0;
    std::cout << "#" << std::setw(10) << counter++
        << " pts= "<< std::setw(10) << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::microseconds(result->pts)).count()
        << " metric= " << std::setw(10) << result->mismatch_percentage << std::endl;
    statistic_t& statistic = *reinterpret_cast<statistic_t*>(user_data->opaque);
    if(result->mismatch_percentage > 0.5){
        statistic.nosync_samples++;
    } else {
        statistic.sync_samples++;
    }
};

void log_error(neuon_logging_t *log, const char *str){

}

void log_info(neuon_logging_t *log, const char *str) {

}

void log_debug(neuon_logging_t *log, const char *str) {

}

int main(int argc, char **argv) {
    std::string media_filename;
    std::string model_filename("model.pb");
    std::string norma_filename("normale.json");

    std::string face_landmark_db_filename("shape_predictor_68_face_landmarks.dat");
    std::string license_filename("neuon.key");

    std::cout << "neuon-example :: " << version << " :: " << birthday << std::endl;
    namespace po = boost::program_options;
    po::options_description opt_desc("Options");
    opt_desc.add_options()
                ("help,h", "Produce this message")
                ("input-media,i", po::value(&media_filename)->required(), "Input media clip to detect A/V sync")
                ("model,m", po::value(&model_filename)->required()->default_value(model_filename), "Neural Network Model")
                ("normalization,n", po::value(&norma_filename)->required()->default_value(norma_filename), "Neural Network Normalization Set")
                ("landmarks,l", po::value(&face_landmark_db_filename)->required()->default_value(face_landmark_db_filename), "Face landmarks DB")
                ("license", po::value(&license_filename)->default_value(license_filename), "License file");


    //logging::info::sink() = std::make_shared<logging::boost_sink_t>();


    po::positional_options_description pos_opt_desc;
    po::variables_map varmap;
    if (!options::is_args_valid(argc, argv, opt_desc, pos_opt_desc, varmap, std::cerr, std::cout)) {
        return 1;
    }

    std::unique_ptr<neuon_context_t, decltype(&neuon_free_engine)> neuon{nullptr, neuon_free_engine};

    const uint64_t target_audio_samplerate = neuon_estimate_target_audio_samplerate(video_frame_duration(media_filename).count());
    const size_t target_video_height = 720;

    extraction_t extraction(media_filename, target_video_height, target_audio_samplerate,
        [&neuon](ffmpeg::frame_ptr frame, const std::chrono::microseconds & pts){
            neuon_put_video(neuon.get(), frame->data[0], frame->linesize[0] * frame->height, frame->width, frame->height, frame->linesize[0], pts.count());
        },
        [&neuon](ffmpeg::frame_ptr sample, const std::chrono::microseconds & pts){
            neuon_put_audio(neuon.get(), sample->data[0], sample->linesize[0], sample->nb_samples, pts.count());
        }
    );

    statistic_t statistic{};

    neuon_user_data_t user_data{};
    user_data.opaque = &statistic;
    user_data.on_result = print_result;

    neuon_options_t options{};
    options.target_audio_samplerate = target_audio_samplerate;

    neuon_logging_t logging{};
    logging.opaque = nullptr;
    logging.error = log_error;
    logging.info = log_info;
    logging.debug = log_debug;

    neuon.reset(neuon_create_engine(model_filename.c_str(), norma_filename.c_str(), face_landmark_db_filename.c_str(), license_filename.c_str(), &options, &logging, &user_data));

    threads::interruption_t interruption;
    posix::sighandler_t<SIGINT>::assign([&interruption](int signal) { interruption.interrupt(); });
    posix::sighandler_t<SIGTERM>::assign([&interruption](int signal) { interruption.interrupt(); });

    extraction.src->run(interruption);

    std::cout << "Checked " << statistic.sync_samples + statistic.nosync_samples << " data samples " << "The stream has " << ((statistic.sync_samples < statistic.nosync_samples) ? "no " : "") << "A/V sync " << "(S:" << statistic.sync_samples << ", N:" <<statistic.nosync_samples << ")";
    return 0;
}
