#pragma once

#include <string>

namespace rykvlv {

class ArticleEvent {
public:
    ArticleEvent(long long chatId, const std::string& articleTheme) : m_chatId(chatId), m_articleTheme(articleTheme) {}

    long long GetChatId() const { return m_chatId; }
    const std::string& GetArticleTheme() const { return m_articleTheme; }

private:
    long long m_chatId;
    std::string m_articleTheme;
};

}