#include <Bot/Bot.h>
#include <iostream>
#include <stdio.h>

namespace rykvlv {

Bot::Bot(const std::string& token) {
    m_tgBot = new TgBot::Bot(token); //std::make_unique<TgBot::Bot>(token);
    gptService = new GptService("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");
}

void Bot::SetupEvents() {
    m_tgBot->getEvents().onCommand("start", [*this](TgBot::Message::Ptr message){
        m_tgBot->getApi().sendMessage(message->chat->id, "Hi!");
    });

    m_tgBot->getEvents().onAnyMessage([*this](TgBot::Message::Ptr message) {
        std::cout << "User @" << message->chat->username << " wrote:" << std::endl;
        std::cout << message->text << std::endl;
        if (message->text.find('\'') != std::string::npos ||
        message->text.find('`') != std::string::npos ||
        message->text.find('~') != std::string::npos ||
        message->text.find('\n') != std::string::npos ||
        message->text.size() > 1000)
            return;
        //printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        rykvlv::GptService::GptResponse gptResponse =  gptService->Promt(message->text);
        if (gptResponse.problem == 12) {
            std::cout << "Error occured" << std::endl;
            return;
        }
            
        std::string textResponse = gptResponse.choices[0].message.content;
        std::cout << "Response: " << textResponse << std::endl;
        m_tgBot->getApi().sendMessage(message->chat->id, textResponse);
    });
}

void Bot::StartPolling() {
    try {
        printf("Bot username: %s\n", m_tgBot->getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(*m_tgBot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
}

// void Bot::StartWebhookServer() {
//     try {
//         printf("Bot username: %s\n", m_tgBot->getApi().getMe()->username.c_str());

//         TgBot::TgWebhookTcpServer webhookServer(4040, *m_tgBot);

//         printf("Server starting\n");
//         m_tgBot->getApi().setWebhook("https://8d4b-85-103-104-250.eu.ngrok.io");
//         webhookServer.start();
//     } catch (std::exception& e) {
//         printf("error: %s\n", e.what());
//     }
// }

}



