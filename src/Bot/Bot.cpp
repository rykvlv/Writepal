#include <Bot/Bot.h>
#include <iostream>
#include <stdio.h>
#include <System/EventManager/Events/PromptEvent.hpp>
#include <System/EventManager/Events/ArticleEvent.hpp>

namespace rykvlv {

Bot::Bot(const std::string& token) : m_token(token) {
    m_tgBot = new TgBot::Bot(token);
    std::string gptToken = std::getenv("GPT3_APT_KEY");
    gptService = new GptService(gptToken);
    m_longPoll = new TgBot::TgLongPoll(*m_tgBot);
    m_eventManager = new EventManager();
}

Bot::~Bot() {
    //delete m_tgBot;
    //delete m_longPoll;
    //delete m_eventManager;
}

void Bot::RegisterEvents() {
    m_eventManager->RegisterHandler("Prompt", [*this](const void* eventData) {
        auto promptEvent = static_cast<const PromptEvent*>(eventData);

        std::optional<std::string> gptResponse = gptService->Prompt(promptEvent->GetChatId(), promptEvent->GetMessage());
        if (!gptResponse.has_value()) {
            std::cout << "Error: Bot::Event::PromptEvent: Error occured on message -> " << promptEvent->GetMessage() << std::endl;
        }

        std::string textResponse = gptResponse.value();
        m_tgBot->getApi().sendMessage(promptEvent->GetChatId(), textResponse);
    });

    m_eventManager->RegisterHandler("Article", [*this](const void* eventData) {
        auto articleEvent = static_cast<const ArticleEvent*>(eventData);
        std::string response = gptService->WriteArticle(articleEvent->GetArticleTheme());
        std::string filename = "articles/" + articleEvent->GetArticleTheme() + ".txt";
        std::ofstream articleFile(filename);
        if (articleFile.is_open()) {
            articleFile << response;
            articleFile.close();
        } else {
            m_tgBot->getApi().sendMessage(articleEvent->GetChatId(), "Ошибка при формировании файла");
        }

        std::string chatId = std::to_string(articleEvent->GetChatId());
        Bot::sendFileToChat(chatId, m_token, filename);
    });
}

void Bot::SetupEvents() {
    m_tgBot->getEvents().onAnyMessage([&](TgBot::Message::Ptr message) {
        if (message->text == "/prompt") {
            m_currentState = EBotState::EPrompt;
            m_tgBot->getApi().sendMessage(message->chat->id, "Bot switched to prompt mode.");
            return;
        }
        if (message->text == "/article") {
            m_currentState = EBotState::EArticle;
            m_tgBot->getApi().sendMessage(message->chat->id, "Bot switched to article mode. Please write the article topic.");
            return;
        }
        if (message->text == "/articles") {
            // handle articles command
            return;
        }
        if (message->text == "/clear") {
            // handle clear command
            return;
        }

        switch (m_currentState) {
            case EBotState::EPrompt: {
                PromptEvent promptEvent(static_cast<long long>(message->chat->id), message->text);
                m_eventManager->TriggerEvent("Prompt", &promptEvent);
                std::cout << "Prompt mode" << std::endl;
                break;
            }
            case EBotState::EArticle: {
                std::thread([&](){
                    ArticleEvent articleEvent(static_cast<long long>(message->chat->id), message->text);
                    m_eventManager->TriggerEvent("Article", &articleEvent);
                }).detach();
                
                std::cout << "Article mode" << std::endl;
                break;
            }
            case EBotState::EArticles: {
                break;
            }
            case EBotState::EClear: {
                break;
            }
            default: {

            }
        }
    });

    m_tgBot->getEvents().onCommand("start", [*this](TgBot::Message::Ptr message){
        m_tgBot->getApi().sendMessage(message->chat->id, "Привет! Я - Writepal, бот-помощник на базе ChatGPT-3.");
    });
}

void Bot::UpdatePolling() {
    try {
        printf("Bot username: %s\n", m_tgBot->getApi().getMe()->username.c_str());
        printf("Long poll started\n");
        m_longPoll->start();
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
}

bool Bot::sendFileToChat(const std::string& chatId, const std::string& token, const std::string& filePath) {
        // Open file stream
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return false;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read file contents into buffer
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Error reading file." << std::endl;
        return false;
    }
    
    // Close file stream
    file.close();
    
    // Create cURL handle and set options
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api.telegram.org/bot" + token + "/sendDocument";
        
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_name(part, "document");
        curl_mime_data(part, buffer.data(), buffer.size());
        curl_mime_type(part, "application/octet-stream");
        curl_mime_filename(part, filePath.c_str());
        
        // Add chat_id field to form data
        curl_mimepart* chatIdPart = curl_mime_addpart(mime);
        curl_mime_name(chatIdPart, "chat_id");
        curl_mime_data(chatIdPart, chatId.c_str(), CURL_ZERO_TERMINATED);
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        
        // Send request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
        std::cerr << "Error sending file: " << curl_easy_strerror(res) << std::endl;
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        return false;
        } else {
        std::cout << "Successfully sent file to chat." << std::endl;
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        return true;
        }
    } else {
        std::cerr << "Error initializing cURL." << std::endl;
        return false;
    }
}

}



