// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_YOUTUBE_PLAYLIST_H_
#define INCLUDE_YOUTUBE_PLAYLIST_H_

#include "channel.h"
#include "youtube.h"
#include "json/json.hpp"

#include <string>

using json = nlohmann::json;

namespace Youtube {

    class Playlist : public VideoList {
     public:
        static Playlist Get(const std::string& p_id);

        inline std::string GetChannelId() const { return Check(GetStatus()) ? GetItem()["snippet"]["channelId"] : ""; }
        inline std::string GetPlaylistId() const override { return GetId(); };

     private:
        Playlist() = default;

        explicit Playlist(const json& p_json_result)
            : VideoList(p_json_result) { Youtube::Channel::Get(GetChannelId(), false); }

        static bool GetOnline(Playlist* p_playlist, const std::string& p_id);
        static bool GetOffline(Playlist* p_playlist, const std::string& p_id);
    };
} // namespace Youtube

#endif // INCLUDE_YOUTUBE_PLAYLIST_H_
