// Copyright 2018 Linus Klöckner

#include "YoutubeListDownloader/youtube/video.h"

#include "YoutubeDecipher/decipher.h"
#include "YoutubeListDownloader/download/downloader.h"
#include "YoutubeListDownloader/logger.h"
#include "YoutubeListDownloader/sql/dbs.h"
#include "YoutubeListDownloader/youtube/format.h"
#include "nowide/fstream.hpp"
#include "nowide/cstdio.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <cpr/cpr.h>
#include <fstream>
#include <string>
#include <vector>

Youtube::Video Youtube::Video::Get(const std::string& p_id) {
    Video video;

    if (!GetOffline(video, p_id))
        GetOnline(video, p_id);

    return video;
}

bool Youtube::Video::GetOnline(Video& p_video, const std::string& p_id) {

    auto response = cpr::Get(cpr::Url{"https://www.googleapis.com/youtube/v3/videos?id=" + p_id +
                                      "&part=snippet&key=" + Settings::Instance().GetApiKey()},
                             cpr::VerifySsl{false}); // cpr currently not supports ca files

    if (json::parse(response.text).is_discarded())
        throw std::runtime_error("Bad response: Json is discarded");

    if (json::parse(response.text)["items"].empty()) {
        LogWarn() << "Video with id \"" << p_id << "\" NOT found online";
        p_video = Video();
        return false;
    }

    p_video = Video(json::parse(response.text));
    SQL::Instance().InsertVideo(p_video);

    return true;
}

bool Youtube::Video::GetOffline(Video& p_video, const std::string& p_id) {
    json json_video = SQL::Instance().QueryVideo(p_id);

    if (json_video["items"].empty()) {
        p_video = Video();
        return false;
    }

    p_video = Video(json_video);

    return true;
}

void Youtube::Video::LoadThumbnail() {
    if (SQL::Instance().ExistsThumbnail(GetId()))
        return;

    LogDebug() << GetId() << " : Load Thumbnail";

    boost::format fmt{"/vi/%1%/default.jpg"};

    fmt % GetId();

    auto thumbnail_response = cpr::Get(cpr::Url{"https://i.ytimg.com/" + fmt.str()},
                                       cpr::VerifySsl{false}); // cpr currently not supports ca files

    SQL::Instance().InsertThumbnail(GetId(), thumbnail_response.text);
}

void Youtube::Video::LoadDownloadLinks() {
    boost::format fmt{"/watch?v=%1%"};
    fmt % GetId();

    LogDebug() << GetId() << " : Load DL Links";

    auto html_response = cpr::Get(cpr::Url{"https://www.youtube.com/" + fmt.str()},
                                  cpr::VerifySsl{false}); // cpr currently not supports ca files

    // RegExes
    boost::regex expr_stream_map{R"("url_encoded_fmt_stream_map":"(.+?"))"};
    boost::regex expr_adaptive_fmts{R"("adaptive_fmts":"(.+?"))"};
    boost::regex expr_qualities{R"((.+?)(?:\,|"))"};
    boost::regex expr_quality_details{R"((.+?)=(.+?)(?:,|\\u0026|"))"};

    // Iterator ends
    boost::sregex_iterator iter_end;
    boost::sregex_token_iterator iter_end_token;

    // Stream Map
    boost::smatch matches_stream_map;
    boost::regex_search(html_response.text, matches_stream_map, expr_stream_map);
    std::string str_stream_map = matches_stream_map[1];

    // Adaptive Fmts
    boost::smatch matches_adaptive_fmts;
    boost::regex_search(html_response.text, matches_adaptive_fmts, expr_adaptive_fmts);
    std::string str_adaptive_fmts = matches_adaptive_fmts[1];

    // Qualities - Stream Map
    boost::sregex_token_iterator iter_stream_map{str_stream_map.begin(), str_stream_map.end(), expr_qualities};

    // Qualities - Adaptive Fmts
    boost::sregex_token_iterator iter_adaptive_fmts{str_adaptive_fmts.begin(), str_adaptive_fmts.end(), expr_qualities};

    std::vector<std::string> str_qualities;

    while (iter_stream_map != iter_end_token) {
        std::string str_quality = *iter_stream_map++;

        str_qualities.push_back(str_quality);
    }

    while (iter_adaptive_fmts != iter_end_token) {
        std::string str_quality = *iter_adaptive_fmts++;

        str_qualities.push_back(str_quality);
    }

    // QualityDetails
    for (auto str_quality : str_qualities) {
        boost::sregex_iterator iter_quality_details{str_quality.begin(), str_quality.end(), expr_quality_details};

        std::string itag = "0";
        std::string url;
        std::string signature;

        while (iter_quality_details != iter_end) {
            auto matches_quality_detail = *iter_quality_details++;

            if (matches_quality_detail[1].compare("itag") == 0)
                itag = matches_quality_detail[2];

            if (matches_quality_detail[1].compare("url") == 0)
                url = matches_quality_detail[2];

            if (matches_quality_detail[1].compare("s") == 0)
                signature = matches_quality_detail[2];
        }

        if (url.empty())
            continue;

        boost::replace_all(url, "%25", "%");
        boost::replace_all(url, "%26", "&");
        boost::replace_all(url, "%2C", ",");
        boost::replace_all(url, "%2F", "/");
        boost::replace_all(url, "%3A", ":");
        boost::replace_all(url, "%3D", "=");
        boost::replace_all(url, "%3F", "?");

        if (!signature.empty()) {
            boost::replace_all(signature, "%25", "%");
            boost::replace_all(signature, "%26", "&");
            boost::replace_all(signature, "%2C", ",");
            boost::replace_all(signature, "%2F", "/");
            boost::replace_all(signature, "%3A", ":");
            boost::replace_all(signature, "%3D", "=");
            boost::replace_all(signature, "%3F", "?");

            Decipher::Instance(html_response.text).DecipherSignature(&signature);

            url.append("&sig=");
            url.append(signature);
        }

        m_qualities.push_back(Quality{std::stoi(itag), url});
    }
}

void Youtube::Video::PrintFormats() const {
    auto qualities = GetQualities();
    std::sort(qualities.begin(), qualities.end());

    for (const auto& quality : qualities) {
        Format format = Youtube::g_FORMATS[quality.m_itag];
        LogInfo() << quality.m_itag << ": " << format.GetString();
    }
}

void Youtube::Video::Download(const Video& p_video, const Download::Options& p_options) {
    for (auto itag : p_options.m_itags) {
        auto& video_qualities = p_video.GetQualities();

        auto pred = [itag](const Video::Quality& q) { return q.m_itag == itag; };
        auto quality_iter = std::find_if(video_qualities.begin(), video_qualities.end(), pred);

        if (quality_iter == video_qualities.end())
            continue;
        auto quality = *quality_iter;

        if (g_FORMATS.find(quality.m_itag) == g_FORMATS.end()) {
            LogWarn() << p_video.GetId() << " : Could not find video informations for itag" << quality.m_itag;
            continue;
        }

        Format format = Youtube::g_FORMATS[quality.m_itag];

        std::string filename{p_video.GetId()};
        if (p_options.m_save_video_name)
            filename = p_video.GetTitle();

        filename.append(".");
        filename.append(format.container);

        boost::filesystem::create_directories(p_options.m_path);

        boost::filesystem::path p{p_options.m_path};

        p.append(filename);

        nowide::ifstream file{p.string()};

        if (file) {
            file.close();
            LogInfo() << p_video.GetId() << " : Video already downloaded";
            return;
        }

        LogInfo() << p_video.GetId() << " : Download - " << format.GetString();

        auto downloader = FileDownloader::Instance();
        downloader.AttachCallback([](double p_dl_now_mb, double p_dl_total_mb, double p_speed_mbs) {
            std::cout << std::fixed << std::setprecision(2) << "DOWN: " << p_dl_now_mb << " mb of " << p_dl_total_mb << " mb speed " << p_speed_mbs << " mb/s \r";
            std::cout.flush();
        });
        downloader.Download(quality.m_url, p.string());

        // Check if something was downloaded
        if (downloader.GetProgress().total_dl == 0) {
            LogWarn() << "Nothing was downloaded! URL may be corrupt:"
                      << "\nURL: " << quality.m_url;
            nowide::remove(p.string().c_str());
        }

        return;
    }
    LogWarn() << p_video.GetId() << " : Could not find suitable itag for video";
}
