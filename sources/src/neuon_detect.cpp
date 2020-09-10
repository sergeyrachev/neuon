// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "version.h"
#include "birthday.h"
#include "logging.h"
#include "options.h"
#include "demo.h"
#include "license.h"
#include "interruption.h"
#include "signal_handler.h"
#include "extraction.h"
#include "model.h"
#include "tensorflow_api.h"
#include "tensorflow_static.h"
#include "tensorflow_dynamic.h"

#include <chrono>
#include <iostream>

#include <boost/filesystem.hpp>

int main(int argc, char **argv){
    std::string media_filename;
    std::string model_filename;

    std::string face_landmark_db("shape_predictor_68_face_landmarks.dat");
    std::string license_filename("neuon.key");

    std::cout << "neuon-detect :: " << version << " :: " << birthday << std::endl;
    namespace po = boost::program_options;
    po::options_description opt_desc( "Options" );
    opt_desc.add_options( )
                ( "help,h", "Produce this message" )
                ( "input-media,i", po::value(&media_filename)->required(), "Input media clip to detect A/V sync" )
                ( "model,m", po::value(&model_filename)->required(), "Input media clip to detect A/V sync" )
                ( "landmarks,l", po::value(&face_landmark_db)->required()->default_value(face_landmark_db), "Face landmarks DB")
                ( "license,", po::value(&license_filename), "License file")
        ;

    logging::verbosity = logging::verbosity_t::info;

    po::positional_options_description pos_opt_desc;
    po::variables_map varmap;
    if( !options::is_args_valid(argc, argv, opt_desc, pos_opt_desc, varmap, std::cerr, std::cout ) ) {
        return 1;
    }

    std::string license_payload;
    std::ifstream license_file(license_filename);
    if(license_file){
        std::getline(license_file, license_payload);
    }

    common::license_t license(license_payload);
    if(license.demo()){
        try{
            common::demo_t demo;
        } catch (const common::demo_exception_t & e ){
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }

    std::ifstream model_file(model_filename, std::ios::binary);
    if(!model_file){
        logging::debug() << "Model content is not available. check the file " << model_filename;
        return 1;
    }

    std::vector<uint8_t> payload(boost::filesystem::file_size(model_filename));
    model_file.read(reinterpret_cast<char *>(payload.data()), payload.size());

    std::unique_ptr<tensorflow::api_t> tensorflow(new tensorflow::static_backend_t());
    neuon::model_t model(std::shared_ptr<tensorflow::api_t>(std::move(tensorflow)), payload);

    extraction_t extraction(media_filename, face_landmark_db, [&model](
        const dlib::matrix<uint8_t>& video_set,
        const std::chrono::microseconds& first_video_pts,
        const std::chrono::microseconds& last_video_pts,
        const dlib::matrix<double>& audio_set,
        const std::chrono::microseconds& first_audio_pts,
        const std::chrono::microseconds last_audio_pts){

        static uint32_t counter = 0;
        auto prediction = model.predict(video_set, {1, 9, 60, 100, 1}, audio_set, {1, 3, 45, 15, 1});
        logging::info() << "Video sample #" << counter << " at " << first_video_pts.count() << " is " << prediction[0] * 100 << " % " << " differs from audio. Is in sync? " << bool(prediction[0] < 0.5);
        counter++;


    });

    interruption_t interruption;
    signal_handler_tt<SIGINT>([&interruption](int signal){interruption.interrupt();});
    signal_handler_tt<SIGTERM>([&interruption](int signal){interruption.interrupt();});

    extraction.src->run(interruption);

    return 0;
}
