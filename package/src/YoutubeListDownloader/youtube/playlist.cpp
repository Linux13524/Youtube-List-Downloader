// Copyright 2018 Linus Klöckner

#include "YoutubeListDownloader/youtube/playlist.h"

#include "YoutubeListDownloader/logger.h"
#include "YoutubeListDownloader/sql/dbs.h"

#include <cpr/cpr.h>
#include <string>

Youtube::Playlist Youtube::Playlist::Get(const std::string& p_id) {
    Playlist playlist;

    if (!GetOffline(&playlist, p_id))
        GetOnline(&playlist, p_id);

    return playlist;
}

bool Youtube::Playlist::GetOnline(Playlist* p_playlist, const std::string& p_id) {
    auto response = cpr::Get(cpr::Url{"https://www.googleapis.com/youtube/v3/playlists?id=" + p_id +
                                      "&part=snippet&key=" + Settings::Instance().GetApiKey()},
                             cpr::VerifySsl{false});   // cpr currently not supports ca files

    if (json::parse(response.text).is_discarded())
        throw std::runtime_error("Bad response: Json is discarded");

    if (json::parse(response.text)["items"].empty()) {
        LogWarn() << "Playlist with id \"" << p_id << "\" NOT found online";
        *p_playlist = Playlist();
        return false;
    }

    *p_playlist = Playlist(json::parse(response.text));
    SQL::Instance().InsertPlaylist(*p_playlist);

    return true;
}

bool Youtube::Playlist::GetOffline(Playlist* p_playlist, const std::string& p_id) {
    json json_playlist = SQL::Instance().QueryPlaylist(p_id);

    if (json_playlist["items"].empty()) {
        *p_playlist = Playlist();
        return false;
    }

    *p_playlist = Playlist(json_playlist);

    return true;
}
