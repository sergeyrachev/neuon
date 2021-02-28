// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "version.h"
#include "birthday.h"
#include "options.h"
#include "demo.h"
#include "license.h"
#include "interruption.h"
#include "sighandler.h"
#include "extraction.h"
#include "composer.h"
#include "face_detection.h"
#include "speech_detection.h"
#include "configuration.h"
#include "neuon_cxx.h"

#include <chrono>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

int main(int argc, char *argv[]) {
    std::string media_filename;
    std::string landmark_filepath("shape_predictor_68_face_landmarks.dat");
    int32_t distance{0};
    std::string output_filename;

    std::string license_filename("neuon.key");

    std::cout << "neuon-data :: " << neuon::version << " :: " << neuon::birthday << std::endl;
    namespace po = boost::program_options;
    po::options_description opt_desc( "Options" );
    opt_desc.add_options( )
                ( "help,h", "Produce this message" )
                ( "input-media,i", po::value(&media_filename)->required(), "Input media clip to detect A/V sync" )
                ( "landmarks,l", po::value(&landmark_filepath)->required(), "Face landmarks DB")
                ( "answer,a", po::value(&distance)->required(), "Correct answer for provided input stream")
                ( "output,o", po::value(&output_filename)->required(), "Correct answer for provided input stream")
                ( "license,", po::value(&license_filename), "License file")
        ;

    //logging::info::sink() = std::make_shared<logging::boost_sink_t>();

    po::positional_options_description pos_opt_desc;
    po::variables_map varmap;
    if( !options::is_args_valid(argc, argv, opt_desc, pos_opt_desc, varmap, std::cerr, std::cout ) ) {
        if(varmap.count("help") || varmap.empty()){
            return 0;
        }
        return 1;
    }

    std::string license_payload;
    std::ifstream license_file(license_filename);
    if(license_file){
        std::getline(license_file, license_payload);
    }

    common::license_t license(license_payload);
    if(license.demo()){
        common::demo_t demo(neuon::birthday);
        if(demo.has_expired){
            std::cerr << "Demo time period limit exceeded! Please contact the seller." << std::endl;
            return 1;
        }
    }

    std::ofstream output(output_filename);
    std::ifstream landmark_file(landmark_filepath, std::ios::binary);

    const uint64_t target_audio_samplerate = neuon::target_audio_samplerate(video_frame_duration(media_filename));
    const size_t target_video_height = 720;

    std::unique_ptr<neuon::face_detection_t> face_detection(new neuon::face_detection_t(neuon::video_width_per_entry, neuon::video_height_per_entry, landmark_file));

    const std::chrono::microseconds audio_slice(static_cast<uint64_t>(std::round(neuon::fft_samples_per_entry * 1000000.0 / target_audio_samplerate)));
    std::unique_ptr<neuon::speech_detection_t> speech_detection(new neuon::speech_detection_t(target_audio_samplerate, audio_slice, neuon::audio_slice_overlap, neuon::audio_features_per_entry));

    std::unique_ptr<neuon::composer_t> composer(
        new neuon::composer_t(
            neuon::video_frames_per_entry,
            neuon::audio_samples_per_entry,
            [distance, &output, output_filename](
                const dlib::matrix <uint8_t> &video_set,
                const std::chrono::microseconds &first_video_pts,
                const std::chrono::microseconds &last_video_pts,
                const dlib::matrix<double> &audio_set,
                const std::chrono::microseconds &first_audio_pts,
                const std::chrono::microseconds last_audio_pts) {

                static uint32_t counter = 0;

                std::string video_set_filename = output_filename + ".v." + std::to_string(counter) + ".png";
                dlib::save_png(video_set, video_set_filename);

                std::string audio_set_filename = output_filename + ".a." + std::to_string(counter) + ".txt";
                std::ofstream af(audio_set_filename);
                af << audio_set;

                std::stringstream line_output;
                line_output << distance << " "
                            << video_set_filename << " "
                            << audio_set_filename << " "
                            << first_video_pts.count() << " "
                            << last_video_pts.count() << " "
                            << first_audio_pts.count() << " "
                            << last_audio_pts.count();

                //logging::info() << counter << " " << line_output.str();
                output << line_output.str() << std::endl;

                counter++;
            }
        )
    );

    neuon::neuon_t neuon(std::move(face_detection), std::move(speech_detection), std::move(composer));

    extraction_t extraction(media_filename, target_video_height, target_audio_samplerate,
        [&neuon](ffmpeg::frame_ptr frame, const std::chrono::microseconds & pts){
            neuon::video_sample_t data_sample{};
            data_sample.p = frame->data[0];
            data_sample.bytes = frame->linesize[0] * frame->height;
            data_sample.width = frame->width;
            data_sample.height = frame->height;
            data_sample.stride = frame->linesize[0];
            data_sample.pts = pts;
            neuon.put(data_sample);
        },
        [&neuon](ffmpeg::frame_ptr sample, const std::chrono::microseconds & pts){
            neuon::audio_sample_t data_sample;
            data_sample.p = sample->data[0];
            data_sample.bytes = sample->linesize[0];
            data_sample.samples = sample->nb_samples;
            data_sample.pts = pts;
            neuon.put(data_sample);
    });

    threads::interruption_t interruption;
    posix::sighandler_t<SIGINT>::assign([&interruption](int signal){interruption.interrupt();});
    posix::sighandler_t<SIGTERM>::assign([&interruption](int signal){interruption.interrupt();});

    extraction.src->run(interruption);

    return 0;
}


