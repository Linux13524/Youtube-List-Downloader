// Copyright 2018 Linus Klöckner

#include "YoutubeListDownloader/youtube/channel.h"

#include "YoutubeListDownloader/logger.h"
#include "YoutubeListDownloader/sql/dbs.h"

#include <cpr/cpr.h>
#include <string>

Youtube::Channel Youtube::Channel::Get(const std::string& p_id, const bool& p_is_username) {
    Channel channel;

    if (!GetOffline(&channel, p_id, p_is_username))
        GetOnline(&channel, p_id, p_is_username);

    return channel;
}

bool Youtube::Channel::GetOnline(Channel* p_channel, const std::string& p_id, const bool& p_is_username) {
    std::string id_string{p_is_username ? "forUsername" : "id"};

    auto response = cpr::Get(cpr::Url{"https://www.googleapis.com/youtube/v3/channels?" + id_string + "=" + p_id +
                                      "&part=snippet,contentDetails&key=" + Settings::Instance().GetApiKey()},
                             cpr::VerifySsl{false});   // cpr currently not supports ca files

    if (json::parse(response.text).is_discarded())
        throw std::runtime_error("Bad response: Json is discarded");

    if (json::parse(response.text)["items"].empty()) {
        LogWarn() << "Channel with id \"" << p_id << "\" NOT found online";
        *p_channel = Channel();
        return false;
    }

    *p_channel = Channel(json::parse(response.text));
    SQL::Instance().InsertChannel(*p_channel);

    return true;
}

bool Youtube::Channel::GetOffline(Channel* p_channel, const std::string& p_id, const bool& p_is_username) {
    json json_channel;

    if (p_is_username)
        json_channel = SQL::Instance().QueryUser(p_id);
    else
        json_channel = SQL::Instance().QueryChannel(p_id);

    if (json_channel["items"].empty()) {
        LogInfo() << "Channel with id \"" << p_id << "\" NOT found offline";
        *p_channel = Channel();
        return false;
    }

    *p_channel = Channel(json_channel);

    return true;
}
