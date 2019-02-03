// Copyright 2018 Linus Klöckner

#include "YoutubeListDownloader/youtube/video_list.h"

#include "YoutubeListDownloader/logger.h"
#include "YoutubeListDownloader/sql/dbs.h"
#include "YoutubeListDownloader/utils/working_queue.h"
#include "YoutubeListDownloader/youtube/video.h"

#include <boost/make_unique.hpp>
#include <cpr/cpr.h>
#include <memory>
#include <string>
#include <vector>

using boost::thread;

std::unique_ptr<thread> Youtube::VideoList::LoadVideos() {
    if (m_status == Status::EXCEPTION) {
        throw std::runtime_error(
            "\n\t"
            "Could not load videos from bad list.\n\t"
            "There was an error with Get(...).\n\t"
            "Aborting.");
    }

    if (m_status == Youtube::Status::LOADED) {
        throw std::runtime_error(
            "\n\t"
            "The list is already loaded.\n\t"
            "Check for double execution of 'LoadVideos(...)'");
    }

    if (m_status == Youtube::Status::LOADING) {
        throw std::runtime_error(
            "\n\t"
            "The list is currently loading.\n\t"
            "Check for double execution of 'LoadVideos(...)'.\n\t"
            "Aborting.");
    }

    m_videos.clear();
    m_status = Status::LOADING;

    std::unique_ptr<boost::thread> thread;

    if (HasListChanged()) {
        thread = boost::make_unique<boost::thread>(boost::bind(&Youtube::VideoList::LoadVideosOnline, this, GetPlaylistId()));
    } else {
        thread = boost::make_unique<boost::thread>(boost::bind(&Youtube::VideoList::LoadVideosOffline, this, GetId()));
    }

    return thread;
}

bool Youtube::VideoList::HasListChanged() {
    // TODO: implement function to check for list changes online
    return true;
}

void Youtube::VideoList::LoadVideosOnline(const std::string& p_playlist_id) {
    std::string page_token;

    cpr::Response response;

    auto pool = boost::make_unique<Utils::WorkingQueue>(true, 32);

    do {
        if (!page_token.empty())
            page_token = "&pageToken=" + page_token;

        response = cpr::Get(cpr::Url{"https://www.googleapis.com/youtube/v3/playlistItems?playlistId=" + p_playlist_id +
                                     page_token + "&part=snippet&maxResults=20&key=" + Settings::Instance().GetApiKey()},
                            cpr::VerifySsl{false});   // cpr currently not supports ca files

        if (json::parse(response.text).is_discarded()) {
            LogError() << "Bad response";
            throw std::runtime_error("Json is discarded");
        }

        if (!json::parse(response.text)["nextPageToken"].is_null())
            page_token = json::parse(response.text)["nextPageToken"];

        pool->AddTask(boost::bind(&Youtube::VideoList::LoadVideosPage, this, json::parse(response.text)));
    } while (!json::parse(response.text)["nextPageToken"].is_null());

    pool.reset();

    m_status = Status::LOADED;
}

void Youtube::VideoList::LoadVideosOffline(const std::string& p_playlist_id) {
    auto video_ids = SQL::Instance().QueryVideoList(p_playlist_id);

    for (const auto& video_id : video_ids) {
        m_videos.push_back(Video::Get(video_id));

        LogInfo() << "Loaded Video " << m_videos.size() << "/" << video_ids.size();
    }

    m_status = Status::LOADED;
}

void Youtube::VideoList::LoadVideosPage(const json& p_json_page) {
    auto count = p_json_page["items"].size();

    for (int i = 0; i < count; i++) {
        std::string current_video_id = p_json_page["items"][i]["snippet"]["resourceId"]["videoId"];
        int current_video_position = p_json_page["items"][i]["snippet"]["position"];

        SQL::Instance().InsertVideoList(current_video_id, GetId(), current_video_position);

        Video video = Video::Get(current_video_id);

        video.LoadThumbnail();
        video.LoadDownloadLinks();

        m_videos.push_back(video);

        LogInfo() << "Loaded Video " << m_videos.size() << "/" << p_json_page["pageInfo"]["totalResults"];
    }
}

void Youtube::VideoList::DownloadVideos(std::vector<int> p_qualities,
                                        const std::function<void(size_t, size_t)>& p_f,
                                        const boost::filesystem::path& p_folder) {
    auto videos = GetVideos();

    auto f_progress = [&p_f, &videos](size_t progress) {
        p_f(videos.size() - progress, videos.size());
    };

    // curl (cpr) creates error when multithreaded
    Utils::WorkingQueue pool(false, 1, f_progress);

    for (auto& video : videos) {
        if (p_folder.empty())
            pool.AddTask(boost::bind(&Youtube::Video::Download, video, p_qualities));
        else
            pool.AddTask(boost::bind(&Youtube::Video::Download, video, p_qualities, p_folder));
    }
    pool.Start();
}
