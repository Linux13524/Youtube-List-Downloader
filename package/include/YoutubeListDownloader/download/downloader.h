
#ifndef INCLUDE_DOWNLOAD_DOWNLOADER_H
#define INCLUDE_DOWNLOAD_DOWNLOADER_H

#include <curl/curl.h>
#include <functional>
#include <string>

class FileDownloader {
public:
    static FileDownloader& Instance() {
        static FileDownloader s_instance{};
        return s_instance;
    }

    ~FileDownloader() {
        curl_global_cleanup();
    }

    void Download(const std::string& p_url, const std::string& p_path);
    void AttachCallback(const std::function<void(double, double, double)>& p_callback) {
        m_progress.callback = p_callback;
    }

private:
    FileDownloader() {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    static size_t WriteData(char* p_ptr, size_t p_size, size_t p_nmemb, void* p_stream);

    static int ProgressCallback(void* p_progress,
                                curl_off_t p_dl_total, curl_off_t p_dl_now,
                                curl_off_t p_ul_total, curl_off_t p_ul_now);

    struct Progress {
        double last_runtime = 0;
        double last_dl = 0;
        CURL* curl = nullptr;
        std::function<void(double, double, double)> callback = nullptr;
    } m_progress;
};

#endif   //INCLUDE_DOWNLOAD_DOWNLOADER_H
