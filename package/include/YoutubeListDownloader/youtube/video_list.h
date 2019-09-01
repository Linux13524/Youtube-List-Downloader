// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_YOUTUBE_VIDEO_LIST_H_
#define INCLUDE_YOUTUBE_VIDEO_LIST_H_

#include "YoutubeListDownloader/download/options.h"
#include "YoutubeListDownloader/utils/working_queue.h"
#include "YoutubeListDownloader/youtube/youtube.h"
#include "json/json.hpp"

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace Youtube {

    class Video;

    class VideoList {
     public:
        inline const std::string GetId() const { return Check(m_status) ? m_item["id"] : ""; }
        inline const std::string GetTitle() const { return Check(m_status) ? m_item["snippet"]["title"] : ""; }
        virtual const std::string GetPlaylistId() const = 0;

        std::unique_ptr<boost::thread> LoadVideos();
        void DownloadVideos(const std::function<void(size_t, size_t)>& p_f = [](size_t, size_t) {},
                            const Download::Options& p_options = Download::Options::GlobalOptions());

        json GetItem() const { return m_item; }
        const std::vector<Video>& GetVideos() const { return m_videos; }
        Status GetStatus() const { return m_status; }

     protected:
        VideoList() = default;

        explicit VideoList(const json& p_json_result)
            : m_item(p_json_result["items"][0])
            , m_status(Status::OK) {}

     private:
        json m_item{json::parse("{}")};
        std::vector<Video> m_videos;
        Status m_status{Status::EXCEPTION};

        bool HasListChanged();

        void LoadVideosOnline(const std::string& p_playlist_id);
        void LoadVideosOffline(const std::string& p_playlist_id);

        void LoadVideosPage(const json& p_json_page);
    };

}   // namespace Youtube

#endif   // INCLUDE_YOUTUBE_VIDEO_LIST_H_
