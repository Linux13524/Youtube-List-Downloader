// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_YOUTUBE_FORMAT_H_
#define INCLUDE_YOUTUBE_FORMAT_H_

#include "YoutubeListDownloader/logger.h"

#include <boost/format.hpp>
#include <map>
#include <string>

namespace Youtube {

    struct Format {
        std::string container;
        std::string audio_codec;
        std::string video_codec;
        int video_resolution;
        int audio_bitrate;

        std::string GetString() {
            if (audio_codec.empty())
                return (boost::format("%1%: Video-Only: %1%: %2% %3%p") % container % video_codec % video_resolution).str();
            else if (video_codec.empty())
                return (boost::format("%1%: Audio-Only:  %2% @ %3% kb/s") % container % audio_codec % audio_bitrate).str();
            else
                return (boost::format("%1%: %2% %3%p - %4% @ %5% kb/s") % container % video_codec % video_resolution % audio_codec % audio_bitrate).str();
        }
    };

    static std::map<int, Format> g_FORMATS = {

        {5, {"flv", "mp3", "H.263", 144, 64}},
        {6, {"flv", "mp3", "H.263", 240, 64}},
        {13, {"3gp", "aac", "MPEG-4", 144, 0}},
        {17, {"3gp", "aac", "MPEG-4", 144, 24}},
        {18, {"mp4", "aac", "H.264", 360, 96}},
        {22, {"mp4", "aac", "H.264", 720, 192}},
        {34, {"flv", "aac", "H.264", 360, 128}},
        {35, {"flv", "aac", "H.264", 480, 128}},
        {36, {"3gp", "aac", "MPEG-4", 240, 0}},
        {37, {"mp4", "aac", "H.264", 1080, 192}},
        {38, {"mp4", "aac", "H.264", 3072, 192}},
        {43, {"webm", "vorbis", "VP8", 360, 128}},
        {44, {"webm", "vorbis", "VP8", 480, 128}},
        {45, {"webm", "vorbis", "VP8", 720, 192}},
        {46, {"webm", "vorbis", "VP8", 1080, 192}},
        {59, {"mp4", "aac", "H.264", 480, 128}},
        {78, {"mp4", "aac", "H.264", 480, 128}},
        {82, {"mp4", "aac", "H.264", 360, 128}},
        {83, {"mp4", "aac", "H.264", 480, 128}},
        {84, {"mp4", "aac", "H.264", 720, 192}},
        {85, {"mp4", "aac", "H.264", 1080, 192}},
        {91, {"mp4", "aac", "H.264", 144, 48}},
        {92, {"mp4", "aac", "H.264", 240, 48}},
        {93, {"mp4", "aac", "H.264", 360, 128}},
        {94, {"mp4", "aac", "H.264", 480, 128}},
        {95, {"mp4", "aac", "H.264", 720, 256}},
        {96, {"mp4", "aac", "H.264", 1080, 256}},
        {100, {"webm", "vorbis", "VP8", 360, 128}},
        {101, {"webm", "vorbis", "VP8", 480, 192}},
        {102, {"webm", "vorbis", "VP8", 720, 192}},
        {132, {"mp4", "aac", "H.264", 240, 48}},
        {151, {"mp4", "aac", "H.264", 144, 24}},

        // Video-only (mp4}
        {133, {"mp4", "", "H.264", 240, 0}},
        {134, {"mp4", "", "H.264", 360, 0}},
        {135, {"mp4", "", "H.264", 480, 0}},
        {136, {"mp4", "", "H.264", 720, 0}},
        {137, {"mp4", "", "H.264", 1080, 0}},
        {138, {"mp4", "", "H.264", 4320, 0}},
        {160, {"mp4", "", "H.264", 144, 0}},
        {212, {"mp4", "", "H.264", 480, 0}},
        {213, {"mp4", "", "H.264", 480, 0}},
        {214, {"mp4", "", "H.264", 720, 0}},
        {215, {"mp4", "", "H.264", 720, 0}},
        {216, {"mp4", "", "H.264", 1080, 0}},
        {217, {"mp4", "", "H.264", 1080, 0}},
        {264, {"mp4", "", "H.264", 1440, 0}},
        {266, {"mp4", "", "H.264", 2160, 0}},
        {298, {"mp4", "", "H.264", 720, 0}},
        {299, {"mp4", "", "H.264", 1080, 0}},
        {394, {"mp4", "", "AV1", 144, 0}},
        {395, {"mp4", "", "AV1", 240, 0}},
        {396, {"mp4", "", "AV1", 360, 0}},
        {397, {"mp4", "", "AV1", 480, 0}},
        {398, {"mp4", "", "AV1", 720, 0}},

        // Video-only (webm}
        {167, {"webm", "", "VP8", 360, 0}},
        {168, {"webm", "", "VP8", 480, 0}},
        {169, {"webm", "", "VP8", 720, 0}},
        {170, {"webm", "", "VP8", 1080, 0}},
        {218, {"webm", "", "VP8", 480, 0}},
        {219, {"webm", "", "VP8", 480, 0}},
        {242, {"webm", "", "VP9", 240, 0}},
        {243, {"webm", "", "VP9", 360, 0}},
        {244, {"webm", "", "VP9", 480, 0}},
        {245, {"webm", "", "VP9", 480, 0}},
        {246, {"webm", "", "VP9", 480, 0}},
        {247, {"webm", "", "VP9", 720, 0}},
        {248, {"webm", "", "VP9", 1080, 0}},
        {271, {"webm", "", "VP9", 1440, 0}},
        {272, {"webm", "", "VP9", 2160, 0}},
        {278, {"webm", "", "VP9", 144, 0}},
        {302, {"webm", "", "VP9", 720, 0}},
        {303, {"webm", "", "VP9", 1080, 0}},
        {308, {"webm", "", "VP9", 1440, 0}},
        {313, {"webm", "", "VP9", 2160, 0}},
        {315, {"webm", "", "VP9", 2160, 0}},
        {330, {"webm", "", "VP9", 144, 0}},
        {331, {"webm", "", "VP9", 240, 0}},
        {332, {"webm", "", "VP9", 360, 0}},
        {333, {"webm", "", "VP9", 480, 0}},
        {334, {"webm", "", "VP9", 720, 0}},
        {335, {"webm", "", "VP9", 1080, 0}},
        {336, {"webm", "", "VP9", 1440, 0}},
        {337, {"webm", "", "VP9", 2160, 0}},

        // Audio-only (mp4}
        {139, {"m4a", "aac", "", 0, 48}},
        {140, {"m4a", "aac", "", 0, 128}},
        {141, {"m4a", "aac", "", 0, 256}},
        {256, {"m4a", "aac", "", 0, 0}},
        {258, {"m4a", "aac", "", 0, 0}},
        {325, {"m4a", "aac", "", 0, 0}},
        {328, {"m4a", "aac", "", 0, 0}},

        // Audio-only (webm}
        {171, {"webm", "vorbis", "", 0, 128}},
        {172, {"webm", "vorbis", "", 0, 256}},
        {249, {"webm", "opus", "", 0, 50}},
        {250, {"webm", "opus", "", 0, 70}},
        {251, {"webm", "opus", "", 0, 160}}};
}   // namespace Youtube

#endif   //INCLUDE_YOUTUBE_FORMAT_H_
