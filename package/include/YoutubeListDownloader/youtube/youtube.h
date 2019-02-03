// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_YOUTUBE_YOUTUBE_H_
#define INCLUDE_YOUTUBE_YOUTUBE_H_

#include "YoutubeListDownloader/logger.h"

#include <map>
#include <string>
#include <utility>

namespace Youtube {
    static const char* g_HOST = "www.googleapis.com";

    enum class Status {
        OK,
        LOADING,
        LOADED,
        EXCEPTION,
    };


    inline bool Check(Status p_status) { return (p_status != Status::EXCEPTION); }

    class Settings {
     public:
        static Settings& Instance() {
            static Settings s_instance;
            return s_instance;
        }

        inline void SetCustomApiKey(const std::string& p_api_key) {
            m_current_api_key = p_api_key;
        }

        inline std::string GetApiKey() { return m_current_api_key; }

     private:
        Settings() = default;

        std::string m_current_api_key = "AIzaSyB_mKY2dnABwpbDoj76YHTpsyO9zAHu3l8";
    };

} // namespace Youtube

#endif // INCLUDE_YOUTUBE_YOUTUBE_H_
