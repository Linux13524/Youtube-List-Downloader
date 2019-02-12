
#include "YoutubeListDownloader/utils/downloader.h"
#include <fstream>
#include <nowide_standalone/nowide/fstream.hpp>

void FileDownloader::Download(const std::string& p_url, const std::string& p_path) {

    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        return;
    }

    nowide::ofstream file{p_path, std::ofstream::binary};
    if (!file) {
        curl_easy_cleanup(curl);
        return;
    }

    m_progress.last_runtime = 0;
    m_progress.last_dl = 0;
    m_progress.curl = curl;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, p_url.c_str());
    curl_easy_setopt(curl, CURLOPT_RANGE, "0-");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &m_progress);

    auto r = curl_easy_perform(curl);

    file.close();

    curl_easy_cleanup(curl);
}

size_t FileDownloader::WriteData(char* p_ptr, size_t p_size, size_t p_nmemb, void* p_stream) {
    ((nowide::ofstream*)p_stream)->write(p_ptr, p_nmemb);
    return p_nmemb;
}

int FileDownloader::ProgressCallback(void* p_progress,
                                     curl_off_t p_dl_total, curl_off_t p_dl_now,
                                     curl_off_t p_ul_total, curl_off_t p_ul_now) {
    auto* progress = (struct Progress*)p_progress;
    CURL* curl = progress->curl;
    double cur_time = 0;

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &cur_time);

    double dl_now_mb = p_dl_now / 1024.0 / 1024;
    double dl_total_mb = p_dl_total / 1024.0 / 1024.0;
    double time_diff = cur_time - progress->last_runtime;
    double dl_diff = p_dl_now - progress->last_dl;

    if (time_diff >= 0.1) {
        double speed_mbs = dl_diff / time_diff / 1024.0 / 1024.0;
        progress->last_runtime = cur_time;
        progress->last_dl = p_dl_now;

        if (progress->callback) {
            progress->callback(dl_now_mb, dl_total_mb, speed_mbs);
        }
    }

    return 0;
}
