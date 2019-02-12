#include "YoutubeListDownloader/youtube/video.h"
#include <iostream>

int main() {
    std::cout << "Trying to load video s8XIgR5OGJc" << std::endl;
    Youtube::Video video = Youtube::Video::Get("s8XIgR5OGJc");
    video.LoadDownloadLinks();
    video.PrintFormats();
    Youtube::Video::Download(video, {37, 22, 18});
}