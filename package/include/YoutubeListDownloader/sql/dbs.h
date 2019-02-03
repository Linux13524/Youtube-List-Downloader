// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_SQL_DBS_H_
#define INCLUDE_SQL_DBS_H_

#include "json/json.hpp"

#include <SQLiteCpp/Database.h>
#include <boost/filesystem.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <vector>

using json = nlohmann::json;

namespace Youtube {
    class Video;
    class Channel;
    class Playlist;
}   // namespace Youtube

class SQL {
 public:
    static SQL& Instance() {
#ifdef ANDROID
        boost::filesystem::path p{R"(/data/data/de.linux13524.youtube_list_downloader/databases/)"};
        boost::filesystem::create_directories(p);
#else
        boost::filesystem::path p{boost::filesystem::current_path()};
#endif
        p.append("youtube.db3");
        static SQL s_instance{p.string()};
        return s_instance;
    }

    void CreateTables();

    void InsertVideo(const Youtube::Video& p_video);
    void InsertChannel(const Youtube::Channel& p_channel);
    void InsertPlaylist(const Youtube::Playlist& p_playlist);
    void InsertVideoList(const std::string& p_video_id, const std::string& p_list_id, int p_position);
    void InsertThumbnail(const std::string& p_id, const std::string& p_image);

    json QueryVideo(const std::string& p_video_id);
    json QueryChannel(const std::string& p_channel_id);
    json QueryUser(const std::string& p_user_name);
    json QueryPlaylist(const std::string& p_playlist_id);

    std::vector<std::string> QueryVideoList(const std::string& p_list_id);

    bool ExistsThumbnail(const std::string& p_video_id);

 private:
    explicit SQL(const std::string& p_database_name)
        : m_database(p_database_name, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE) {
        CreateTables();
    }

    boost::shared_mutex m_mutex_videos;
    boost::shared_mutex m_mutex_lists;
    boost::shared_mutex m_mutex_video_lists;
    boost::shared_mutex m_mutex_thumbnails;

    SQLite::Database m_database;
};

#endif   // INCLUDE_SQL_DBS_H_
