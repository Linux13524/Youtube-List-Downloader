// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_UTILS_FS_H_
#define INCLUDE_UTILS_FS_H_

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <string>

namespace Filesystem {
    class Utils {
     public:
        Utils() = delete;

        static inline void ClearForbiddenChars(std::string* p_string) {
            boost::regex expr(R"([\/\\:*?"<>|])");

            // TODO: use similar chars
            *p_string = boost::regex_replace(*p_string, expr, "");
        }
    };

    class Settings {
     public:
        static Settings& Instance() {
            static Settings s_instance;
            return s_instance;
        }

        inline void SetDbPath(const boost::filesystem::path& p_db_path) {
            m_current_db_path = p_db_path;
        }
        inline void SetVideoPath(const boost::filesystem::path& p_video_path) {
            m_current_video_path = p_video_path;
        }

        inline boost::filesystem::path GetDbPath() { return m_current_db_path; }
        inline boost::filesystem::path GetVideoPath() { return m_current_video_path; }

     private:
        Settings() = default;

        boost::filesystem::path m_current_db_path{boost::filesystem::current_path()};
        boost::filesystem::path m_current_video_path{boost::filesystem::current_path().append("videos")};
    };
}   // namespace Filesystem

#endif   // INCLUDE_UTILS_FS_H_
