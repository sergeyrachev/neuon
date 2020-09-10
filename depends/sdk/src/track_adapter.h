
#include <functional>
#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace ffmpeg{
    class track_adapter {
    public:
        explicit track_adapter(const AVStream &stream);

    private:
        std::unique_ptr<AVCodecParameters, std::function<void(AVCodecParameters *)>> _parameters{};
    public:
        const size_t index{0};
        AVCodecParameters &parameters;
    };
}


