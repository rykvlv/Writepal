#pragma once

#include <string>

namespace rykvlv {

class ClearDialogEvent {
public:
    ClearDialogEvent(long long chatId) : m_chatId(chatId) {}

    long long GetChatId() const { return m_chatId; }

private:
    long long m_chatId;
};

}