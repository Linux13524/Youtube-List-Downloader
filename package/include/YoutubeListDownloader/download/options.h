// Copyright (c) 2019 Linus Klöckner

#ifndef INCLUDE_DOWNLOAD_OPTIONS_H
#define INCLUDE_DOWNLOAD_OPTIONS_H

#include <boost/filesystem.hpp>

namespace Download {

    struct Options {
        static Options& GlobalOptions() {
            static Options s_global_options;
            return s_global_options;
        }

        bool m_save_video_name = false;
        boost::filesystem::path m_path = boost::filesystem::current_path();
        std::vector<int> m_itags = {37, 22, 18};
    };

}   // namespace Download

#endif   //INCLUDE_DOWNLOAD_OPTIONS_H
