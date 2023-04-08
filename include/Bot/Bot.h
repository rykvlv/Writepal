#pragma once

#include <string>
#include <tgbot/tgbot.h>
#include <Service/GptService.h>
#include <System/EventManager/EventManager.hpp>

namespace rykvlv {
class Bot {
private:
    enum class EBotState {
        EPrompt = 0, 
        EArticle,
        EArticles, 
        EClear
    };

public:
    Bot() = delete;
    Bot(const std::string& token);
    ~Bot();

    void SetupEvents();
    void RegisterEvents();
    void UpdatePolling();

    static bool sendFileToChat(const std::string& chatId, const std::string& token, const std::string& filePath);
private:
    TgBot::Bot* m_tgBot;
    TgBot::TgLongPoll* m_longPoll;
    GptService* gptService;
    EventManager* m_eventManager;
    EBotState m_currentState = EBotState::EPrompt;

    std::string m_token;
};
}
