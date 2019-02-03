// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_YOUTUBE_CHANNEL_H_
#define INCLUDE_YOUTUBE_CHANNEL_H_

#include "video.h"
#include "video_list.h"
#include "youtube.h"
#include "json/json.hpp"

#include <string>

using json = nlohmann::json;

namespace Youtube {

    class Channel : public VideoList {
     public:
        static Channel Get(const std::string& p_id, const bool& p_is_username);

        inline const std::string GetDescription() const { return Check(GetStatus()) ? GetItem()["snippet"]["description"] : ""; }
        inline const std::string GetPlaylistId() const override { return Check(GetStatus()) ? GetItem()["contentDetails"]["relatedPlaylists"]["uploads"] : ""; }

     private:
        Channel() = default;

        explicit Channel(const json& p_json_result)
            : VideoList(p_json_result) {}

        static bool GetOnline(Channel* p_channel, const std::string& p_id, const bool& p_is_username);
        static bool GetOffline(Channel* p_channel, const std::string& p_id, const bool& p_is_username);
    };
} // namespace Youtube

#endif // INCLUDE_YOUTUBE_CHANNEL_H_
