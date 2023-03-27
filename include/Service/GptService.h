#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <Service/HttpService.h>

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

    GptResponse Promt(const std::string& request);
private:
    void from_json(const json& j, GptResponse& cc);
    std::string m_token;

    HttpService* httpService;
};
}
