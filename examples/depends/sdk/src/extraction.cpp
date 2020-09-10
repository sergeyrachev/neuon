// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "extraction.h"

extraction_t::extraction_t(const std::string &media_filename, size_t target_video_height, size_t target_audio_samplerate, on_resample_callback_t on_video, on_resample_callback_t on_audio) {
    src = std::unique_ptr<source_t>{new source_t(
        media_filename,
        [this, on_video](ffmpeg::frame_ptr au) {
            auto frame_pts = vresample->resample(std::move(au));
            auto& frame = frame_pts.first;
            const auto& pts = frame_pts.second;
            on_video(std::move(frame), pts);
        },
        [this, on_audio](ffmpeg::frame_ptr au) {
            auto frame_pts = aresample->resample(std::move(au));
            auto& frame = frame_pts.first;
            const auto& pts = frame_pts.second;
            on_audio(std::move(frame), pts);
        })};

    const auto &vparams = src->vdec->parameters();
    const auto &aparams = src->adec->parameters();
    aresample = std::unique_ptr<audio_resample_t>(new audio_resample_t(aparams.channel_layout, aparams.sample_fmt, aparams.sample_rate, target_audio_samplerate));
    vresample = std::unique_ptr<video_resample_t>(new video_resample_t(vparams.width, vparams.height, vparams.pix_fmt,
                                                   target_video_height * vparams.width / vparams.height, target_video_height));
}

std::chrono::microseconds video_frame_duration(const std::string &media_filename) {
    ffmpeg::demuxer dmx(media_filename);
    auto tracks = dmx.tracks();
    while (auto au = dmx.get()) {
        auto idx = au->packet->stream_index;
        if (tracks[idx].parameters.codec_type == AVMEDIA_TYPE_VIDEO) {
            return std::chrono::microseconds(au->packet->duration);
        }
    }
    return std::chrono::microseconds(0);
}
