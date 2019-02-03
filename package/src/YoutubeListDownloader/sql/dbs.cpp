// Copyright 2018 Linus Klöckner

#include "YoutubeListDownloader/sql/dbs.h"

#include "YoutubeListDownloader/youtube/channel.h"
#include "YoutubeListDownloader/youtube/playlist.h"

#include <SQLiteCpp/Transaction.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>

void SQL::CreateTables() {
    SQLite::Transaction transaction{m_database};

    m_database.exec(
        "CREATE TABLE IF NOT EXISTS videos ("
        "id TEXT NOT NULL,"
        "title TEXT NOT NULL,"
        "description TEXT NOT NULL,"
        "channelId TEXT NOT NULL,"
        "PRIMARY KEY(id))");

    m_database.exec(
        "CREATE TABLE IF NOT EXISTS lists("
        "id TEXT NOT NULL,"
        "userId TEXT NOT NULL,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "type TEXT NOT NULL,"
        "playlistId TEXT NOT NULL,"
        "channelId TEXT NOT NULL,"
        "PRIMARY KEY(id))");

    m_database.exec(
        "CREATE TABLE IF NOT EXISTS videoLists("
        "listId TEXT NOT NULL,"
        "videoId TEXT NOT NULL,"
        "position INTEGER NOT NULL,"
        "PRIMARY KEY(listId, videoId, position),"
        "FOREIGN KEY(listId) REFERENCES lists(id),"
        "FOREIGN KEY(videoId) REFERENCES videos(id))");

    m_database.exec(
        "CREATE TABLE IF NOT EXISTS thumbnails("
        "id TEXT NOT NULL,"
        "thumbnail BLOB,"
        "PRIMARY KEY(id))");

    transaction.commit();
}

void SQL::InsertVideo(const Youtube::Video& p_video) {
    boost::unique_lock<boost::shared_mutex> lock{m_mutex_videos};

    SQLite::Statement insert_video{m_database, R"(INSERT OR IGNORE INTO videos VALUES (?, ?, ?, ?))"};

    insert_video.bind(1, p_video.GetId());
    insert_video.bind(2, p_video.GetTitle());
    insert_video.bind(3, p_video.GetDescription());
    insert_video.bind(4, p_video.GetChannelId());

    insert_video.exec();
}

void SQL::InsertChannel(const Youtube::Channel& p_channel) {
    boost::unique_lock<boost::shared_mutex> lock{m_mutex_lists};

    SQLite::Statement insert_channel{m_database, R"(INSERT OR IGNORE INTO lists VALUES (?,?,?,?,?,?,?))"};

    std::string user_id = p_channel.GetTitle();
    boost::algorithm::to_lower(user_id);

    insert_channel.bind(1, p_channel.GetId());
    insert_channel.bind(2, user_id);
    insert_channel.bind(3, p_channel.GetTitle());
    insert_channel.bind(4, p_channel.GetDescription());
    insert_channel.bind(5, "channel");
    insert_channel.bind(6, p_channel.GetPlaylistId());
    insert_channel.bind(7, p_channel.GetId());

    insert_channel.exec();
}

void SQL::InsertPlaylist(const Youtube::Playlist& p_playlist) {
    boost::unique_lock<boost::shared_mutex> lock{m_mutex_lists};

    SQLite::Statement insert_playlist{m_database, R"(INSERT OR IGNORE INTO lists VALUES (?,?,?,?,?,?,?))"};

    insert_playlist.bind(1, p_playlist.GetId());
    insert_playlist.bind(2, p_playlist.GetTitle());
    insert_playlist.bind(4, "playlist");
    insert_playlist.bind(5, p_playlist.GetPlaylistId());
    insert_playlist.bind(6, p_playlist.GetChannelId());

    insert_playlist.exec();
}

void SQL::InsertVideoList(const std::string& p_video_id, const std::string& p_list_id, int p_position) {
    boost::unique_lock<boost::shared_mutex> lock{m_mutex_video_lists};

    SQLite::Statement insert_video_list{m_database, R"(INSERT OR IGNORE INTO videoLists VALUES (?, ?, ?))"};

    insert_video_list.bind(1, p_list_id);
    insert_video_list.bind(2, p_video_id);
    insert_video_list.bind(3, p_position);

    insert_video_list.exec();
}

void SQL::InsertThumbnail(const std::string& p_id, const std::string& p_image) {
    boost::unique_lock<boost::shared_mutex> lock{m_mutex_thumbnails};

    SQLite::Statement insert_thumbnail{m_database, R"(INSERT OR IGNORE INTO thumbnails VALUES (?, ?))"};

    insert_thumbnail.bind(1, p_id);
    insert_thumbnail.bind(2, p_image);

    insert_thumbnail.exec();
}

json SQL::QueryVideo(const std::string& p_video_id) {
    boost::shared_lock<boost::shared_mutex> lock{m_mutex_videos};

    SQLite::Statement query_video{m_database, R"(SELECT * FROM videos WHERE id = ?)"};
    query_video.bind(1, p_video_id);

    std::vector<json> videos;

    while (query_video.executeStep()) {
        json json_video = {
            {"id", query_video.getColumn(0).getText()},
            {"snippet",
             {{"title", query_video.getColumn(1).getText()},
              {"description", query_video.getColumn(2).getText()},
              {"channelId", query_video.getColumn(3).getText()}}}};

        videos.push_back(json_video);
    }

    return {{"items", json(videos)}};
}

json SQL::QueryChannel(const std::string& p_channel_id) {
    boost::shared_lock<boost::shared_mutex> lock{m_mutex_lists};

    SQLite::Statement query_channel{m_database, R"(SELECT * FROM lists WHERE id = ? and type = "channel")"};
    query_channel.bind(1, p_channel_id);

    std::vector<json> channels;

    while (query_channel.executeStep()) {

        json json_channel = {
            {"id", query_channel.getColumn(0).getText()},
            {"snippet",
             {{"title", query_channel.getColumn(1).getText()},
              {"description", query_channel.getColumn(2).getText()}}},
            {"contentDetails",
             {{"relatedPlaylists",
               {{"uploads", query_channel.getColumn(5).getText()}}}}}};

        channels.push_back(json_channel);
    }

    return {{"items", json(channels)}};
}

json SQL::QueryUser(const std::string& p_user_name) {
    boost::shared_lock<boost::shared_mutex> lock{m_mutex_lists};

    SQLite::Statement query_channel{m_database, R"(SELECT * FROM lists WHERE userId = ? and type = "channel")"};

    std::string user_name_low{p_user_name};
    boost::algorithm::to_lower(user_name_low);

    query_channel.bind(1, user_name_low);

    std::vector<json> channels;

    while (query_channel.executeStep()) {

        json json_channel = {
            {"id", query_channel.getColumn(0).getText()},
            {"snippet",
             {{"title", query_channel.getColumn(2).getText()},
              {"description", query_channel.getColumn(3).getText()}}},
            {"contentDetails",
             {{"relatedPlaylists",
               {{"uploads", query_channel.getColumn(5).getText()}}}}}};

        channels.push_back(json_channel);
    }

    return {{"items", json(channels)}};
}

json SQL::QueryPlaylist(const std::string& p_playlist_id) {
    boost::shared_lock<boost::shared_mutex> lock{m_mutex_lists};

    SQLite::Statement query_playlist{m_database, R"(SELECT * FROM lists WHERE id = ? and type = "playlist")"};
    query_playlist.bind(1, p_playlist_id);

    std::vector<json> playlists;

    while (query_playlist.executeStep()) {

        json json_playlist = {
            {"id", query_playlist.getColumn(0).getText()},
            {"snippet",
             {{"title", query_playlist.getColumn(1).getText()},
              {"channelId", query_playlist.getColumn(5).getText()}}}};

        playlists.push_back(json_playlist);
    }

    return {{"items", json(playlists)}};
}

std::vector<std::string> SQL::QueryVideoList(const std::string& p_list_id) {
    boost::shared_lock<boost::shared_mutex> lock{m_mutex_lists};

    SQLite::Statement query_video_list{
        m_database,
        R"(SELECT videos.id FROM videos INNER JOIN videoLists ON (videos.id = videoLists.videoId) WHERE videoLists.listId = ? ORDER BY videoLists.position)"};
    query_video_list.bind(1, p_list_id);

    std::vector<std::string> videos;

    while (query_video_list.executeStep()) {
        videos.push_back(query_video_list.getColumn(0).getText());
    }

    return videos;
}

bool SQL::ExistsThumbnail(const std::string& p_video_id) {
    boost::shared_lock<boost::shared_mutex> lock{m_mutex_thumbnails};

    SQLite::Statement query_thumbnail{m_database, R"(SELECT * FROM thumbnails WHERE id = ?)"};
    query_thumbnail.bind(1, p_video_id);

    return query_thumbnail.executeStep();
}
