#pragma once

#include "neuon/export_configuration.h"

#include <cstdlib>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t neuon_microseconds_t;

/// @brief Use this function to get audio samplerate that supported by neural engine as input samplerate.
/// @param video_frame_duration original video frame duration in microseconds
/// @return Samples per second audio sample rate. Audio samples passed to engine should have these sample rate

NEUON_PUBLIC uint64_t neuon_estimate_target_audio_samplerate(neuon_microseconds_t video_frame_duration);

struct NEUON_PUBLIC neuon_outcome_t{
    /// @brief PTS value of the first sample in sequence that wan processed by engine.
    neuon_microseconds_t pts;

    /// @brief Percentage of audio/video mismatching. Values less 0.5 means having sync, greater 0.5 means the analysed sample lose synchronization.
    double mismatch_percentage;
};

struct NEUON_PUBLIC neuon_user_data_t;
/// @brief Callback type to get results
/// @param user_data pointer to this structure
/// @param result contains result of analysis;
using on_result_t = void (*)( neuon_user_data_t *user_data, const neuon_outcome_t *result ) ;

struct NEUON_PUBLIC neuon_user_data_t{
    /// @brief Opaque data to user-defined object
    void* opaque;

    /// @brief Callback is called every data sample after processing. Data sample contains a few video frames in row and corresponding amount of audio samples.
    on_result_t on_result;
};

struct NEUON_PUBLIC neuon_logging_t;
/// @brief Common type for logging callbacks
/// @param log ptr to this structure
/// @param str Message
using log_function_t = void (*)( neuon_logging_t *log, const char *str );

struct NEUON_PUBLIC neuon_logging_t{
    /// @brief Opaque data to user-defined logging object
    void *opaque;

    /// @brief Error messages goes there; In case of unexpected result check the last messages
    log_function_t error;

    /// @brief Informational and general messages goes there; use them for tracking purposes
    log_function_t info;

    /// @brief Debugging messages goes there; be noted - might be a lot of messages for diagnostic purpose. Use in case of issues or weird behaviour
    log_function_t debug;
};

struct NEUON_PUBLIC neuon_options_t{
    /// @brief Estimated audio samplerate that will be used for audio data
    size_t target_audio_samplerate;
};

/// @brief Engine context structure
struct NEUON_PUBLIC neuon_context_t;

/// @brief Creates main functionality core. Engine consumes Gray shaded video and DOUBLE precision audio. To adjust audio to different video framerate you have to resample audio accordingly @see neuon_estimate_target_audio_samplerate returned value.
/// @param model_filename filepath to neural network model in Protobuf format.
/// @param norma_filename normalization coefficient database filepath
/// @param landmark_filename face landmark database filepath
/// @param license_filename license filepath, may be null for demo mode
/// @param options options to set up the core
/// @param user_data callback to result. Must exist before deleting engine. Engine doesn't own it.
/// @return new dynamically allocated instance of engine
NEUON_PUBLIC neuon_context_t* neuon_create_engine(const char *model_filename, const char *norma_filename, const char *landmark_filename, const char* license_filename, const neuon_options_t* options, neuon_logging_t *log, neuon_user_data_t* user_data);

/// @brief Destroys engine
NEUON_PUBLIC void neuon_free_engine(neuon_context_t*);

/// @brief Is used to put video data into engine
/// @param context Engine, created with @see neuon_create_engine
/// @param p pointer to decompressed video data. Video data must be Gray scaled in 255 shades. 8 bit per pixel planar format; Refer to example to see how to convert video
/// @param bytes size of passed data buffer
/// @param width width of frame in pixels
/// @param height height of frame in pixels
/// @param stride width of frame with padding
/// @param pts Presentation timestamp for this frame
NEUON_PUBLIC void neuon_put_video(const neuon_context_t * context, const uint8_t* p, size_t bytes, size_t width, size_t height,  size_t stride, neuon_microseconds_t pts);

/// @brief Is used to put audio data into engine
/// @param context Engine, created with @see neuon_create_engine
/// @param p  pointer to decompressed audio data. Audio data should be resampled accordingly @see neuon_estimate_target_audio_samplerate returned value and presented in DOUBLE precision format. Refer to the example how to resample audio
/// @param bytes size of passed data buffer
/// @param samples how many audio samples provided per buffer
/// @param pts presentation timestamp of the data
NEUON_PUBLIC void neuon_put_audio(const neuon_context_t * context, const uint8_t* p, size_t bytes, size_t samples, neuon_microseconds_t pts);

#ifdef __cplusplus
}
#endif
