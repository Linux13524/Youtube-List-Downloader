#include "YoutubeListDownloader/download/options.h"
#include "YoutubeListDownloader/youtube/video.h"
#include <iostream>

void testDownload() {
    std::cout << "Trying to load video s8XIgR5OGJc" << std::endl;
    Youtube::Video video = Youtube::Video::Get("s8XIgR5OGJc");
    video.LoadDownloadLinks();
    video.PrintFormats();
    Youtube::Video::Download(video);
}

void testDownloadOptions() {

    Download::Options& global_options = Download::Options::GlobalOptions();

    global_options.m_save_video_name = true;
    global_options.m_path = boost::filesystem::current_path().append("videos");
    global_options.m_itags = {37, 22, 18};

    Download::Options new_options{global_options};

    global_options.m_save_video_name = true;
    global_options.m_path = boost::filesystem::current_path().append("videos_new");
    global_options.m_itags = {37, 22, 18, 201};
}

int main() {
    testDownload();
    testDownloadOptions();

    return 0;
}