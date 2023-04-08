#pragma once

#include <string>

namespace rykvlv {

class PromptEvent {
public:
    PromptEvent(long long chatId, const std::string& message) : m_chatId(chatId), m_message(message) {}

    long long GetChatId() const { return m_chatId; }
    const std::string& GetMessage() const { return m_message; }

private:
    long long m_chatId;
    std::string m_message;
};

}