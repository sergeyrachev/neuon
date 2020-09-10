#pragma once

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

#include <fstream>

namespace neuon{
    class face_detection_t {
    public:
        face_detection_t(size_t mouth_width, size_t mouth_height, std::ifstream &db)
            : mouth_width(mouth_width)
            , mouth_height(mouth_height) {
            dlib::deserialize(sp, db);
        }

        dlib::array2d<uint8_t> detect(const dlib::array2d<uint8_t> &img) {

            auto dets = detector(img);
            if (dets.size() == 1) {
                dlib::full_object_detection shape = sp(img, dets[0]);

                auto left_most_x = shape.part(48).x();
                auto right_most_x = shape.part(54).x();

                auto top_most_y = shape.part(52).y();
                auto bottom_most_y = shape.part(57).y();

                for (auto j = 48; j < 68; ++j) {
                    const auto &part = shape.part(j);
                    left_most_x = std::min(left_most_x, part.x());
                    right_most_x = std::max(right_most_x, part.x());

                    top_most_y = std::min(top_most_y, part.y());
                    bottom_most_y = std::max(bottom_most_y, part.y());
                }

                const auto border = 10;
                left_most_x -= border;
                right_most_x += border;
                top_most_y -= border;
                bottom_most_y += border;

                if (left_most_x >= 0 && right_most_x < img.nc() && top_most_y >= 0 && bottom_most_y < img.nr()) {

                    dlib::array2d<uint8_t> mouth_chip(mouth_height, mouth_width);
                    std::array<dlib::dpoint, 4> mouth_bounding_box;
                    mouth_bounding_box[0] = dlib::dpoint(left_most_x, top_most_y);
                    mouth_bounding_box[1] = dlib::dpoint(left_most_x, bottom_most_y);
                    mouth_bounding_box[2] = dlib::dpoint(right_most_x, top_most_y);
                    mouth_bounding_box[3] = dlib::dpoint(right_most_x, bottom_most_y);
                    dlib::extract_image_4points(img, mouth_chip, mouth_bounding_box);

                    return mouth_chip;
                }
            }
            return {};
        }

    private:
        dlib::frontal_face_detector detector{dlib::get_frontal_face_detector()};
        dlib::shape_predictor sp;

        const size_t mouth_width;
        const size_t mouth_height;
    };
}


