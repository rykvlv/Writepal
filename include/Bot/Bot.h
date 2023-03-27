#pragma once

#include <string>
#include <tgbot/tgbot.h>
#include <Service/GptService.h>

namespace rykvlv {
class Bot {
public:
    Bot() = delete;
    Bot(const std::string& token);

    void SetupEvents();
    void StartPolling();
    //void StartWebhookServer();
private:
    TgBot::Bot* m_tgBot;
    GptService* gptService;
};
}
