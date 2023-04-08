#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <Service/HttpService.h>
#include <optional>

using json = nlohmann::json;

namespace rykvlv {
class GptService {

public:
struct GptResponse {
    GptResponse() {
        problem = -1;
    }

    GptResponse(bool problem) {
        problem = 12;
    }

    std::string id;
    std::string object;
    int created;
    std::string model;
    struct Usage {
        int prompt_tokens;
        int completion_tokens;
        int total_tokens;
    } usage;
    struct Choice {
        struct Message {
            std::string role;
            std::string content;
        } message;
        std::string finish_reason;
        int index;
    };
    std::vector<Choice> choices;

    int problem;
};

public:
    GptService(const std::string& token);
    ~GptService();

    std::optional<std::string> Prompt(long long chatId, std::string request);
    //This prompt doesn't affect main chat history
    std::optional<std::string> CleanPrompt(std::string request, std::vector<std::pair<std::string, std::string>>& chatHistory);
    std::string WriteArticle(std::string articleTheme);
private:
    void from_json(const json& j, GptResponse& cc);
    std::string processString(const std::string& s);
    std::string m_token;
    std::unordered_map<long long, std::vector<std::pair<std::string, std::string>>> m_chatHistory;

    HttpService* httpService;
};
}
