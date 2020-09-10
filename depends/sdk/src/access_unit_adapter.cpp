// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "access_unit_adapter.h"

using namespace ffmpeg;

const AVRational access_unit_adapter::Microseconds{std::chrono::microseconds::period::num,
                                                   std::chrono::microseconds::period::den};

access_unit_adapter::access_unit_adapter(packet_ptr pkt, const AVRational& packet_timescale)
    : packet(std::move(pkt))
    , timescale(Microseconds)
{
    av_packet_rescale_ts(packet.get(), packet_timescale, timescale);
}
