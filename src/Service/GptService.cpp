#include <Service/GptService.h>
#include <iostream>


namespace rykvlv {
GptService::GptService(const std::string& token) : m_token(token) {
    httpService = new HttpService();
}

GptService::~GptService() {
    delete httpService;
}

GptService::GptResponse GptService::Promt(const std::string& request) {
    if (request.find('\'') != std::string::npos || 
        request.find('`') != std::string::npos ||
        request.find('~') != std::string::npos)
        return GptService::GptResponse(true);
    std::string url = "https://api.openai.com/v1/chat/completions";
    std::string method = "POST";
    json j = {
        {"model", "gpt-3.5-turbo"},
        {"messages", {
                {"role", "user"},
                {"content", request}
            }
        }
    };
    std::string body = "{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"" + request + "\"}]}";
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        ("Authorization: Bearer " + m_token)
    };
    std::string response = httpService->MakeRequest(url, HttpService::ResponseMethod::POST, body, headers);
    GptService::GptResponse gptResponse;
    from_json(json::parse(response), gptResponse);

    return gptResponse;
}

void GptService::from_json(const json& j, GptResponse& cc) {
    j.at("id").get_to(cc.id);
    j.at("object").get_to(cc.object);
    j.at("created").get_to(cc.created);
    j.at("model").get_to(cc.model);
    j.at("usage").at("prompt_tokens").get_to(cc.usage.prompt_tokens);
    j.at("usage").at("completion_tokens").get_to(cc.usage.completion_tokens);
    j.at("usage").at("total_tokens").get_to(cc.usage.total_tokens);
    for (auto& c : j.at("choices")) {
        GptResponse::Choice choice;
        choice.message.role = c.at("message").at("role").get<std::string>();
        choice.message.content = c.at("message").at("content").get<std::string>();
        c.at("finish_reason").get_to(choice.finish_reason);
        c.at("index").get_to(choice.index);
        cc.choices.push_back(choice);
    }
}
    
}
