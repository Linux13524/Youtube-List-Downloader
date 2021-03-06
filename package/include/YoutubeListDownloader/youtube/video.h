// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_YOUTUBE_VIDEO_H_
#define INCLUDE_YOUTUBE_VIDEO_H_

#include "youtube.h"
#include "json/json.hpp"

#include <boost/filesystem.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace Youtube {

    class Video {
     public:
        struct Quality {
            int m_itag;
            std::string m_url;

            bool operator < (const Quality& q) const
            {
                return (m_itag < q.m_itag);
            }
        };

        static Video Get(const std::string& p_id);

        inline const std::string GetId() const { return Check(m_status) ? m_item["id"] : ""; }
        inline const std::string GetChannelId() const { return Check(m_status) ? m_item["snippet"]["channelId"] : ""; }
        inline const std::string GetTitle() const { return Check(m_status) ? m_item["snippet"]["title"] : ""; }
        inline const std::string GetDescription() const { return Check(m_status) ? m_item["snippet"]["description"] : ""; }
        inline const std::vector<std::string> GetTags() const {
            return Check(m_status) ? m_item["snippet"]["tags"].get<std::vector<std::string>>() : std::vector<std::string>();
        }

        void LoadThumbnail();
        void LoadDownloadLinks();
        void PrintFormats() const;
        static void Download(const Video& p_video, const std::vector<int>& p_itags);
        static void Download(const Video& p_video, const std::vector<int>& p_itags, const boost::filesystem::path& p_folder);

        const std::vector<Quality>& GetQualities() const { return m_qualities; }

     private:
        Video() = default;

        explicit Video(const json& p_json_result)
            : m_item(p_json_result["items"][0])
            , m_status(Status::OK) {}

        static bool GetOnline(Video& p_video, const std::string& p_id);
        static bool GetOffline(Video& p_video, const std::string& p_id);

        json m_item{json::parse("{}")};
        Status m_status{Status::EXCEPTION};
        std::vector<Quality> m_qualities;
    };

} // namespace Youtube

#endif // INCLUDE_YOUTUBE_VIDEO_H_
