#include <Service/GptService.h>
#include <iostream>
#include <regex>


namespace rykvlv {
GptService::GptService(const std::string& token) : m_token(token) {
    httpService = new HttpService();
}

GptService::~GptService() {
    delete httpService;
}

std::optional<std::string> GptService::Prompt(long long from, std::string input) {
    input.erase(std::remove_if(input.begin(), input.end(), [](char c){
        return c == '`' || c == '"' || c == '\'';
    }), input.end());

    std::vector<std::string> messages;
    std::string processedInput = processString(input);
    m_chatHistory[from].push_back(std::make_pair("user", processedInput));
    std::vector<std::pair<std::string, std::string>>& chatHistory = m_chatHistory[from];
    for (auto& [role, message] : chatHistory) {
        std::string processedMessage = processString(message);
        messages.push_back(
            "{\"role\" : \"" + role + "\", \"content\": \"" + processedMessage + "\"}"
        );
    }

    std::ostringstream oss;
    oss << "{\"model\": \"gpt-3.5-turbo\", \"messages\" : [";
    for (size_t i = 0; i < messages.size(); ++i) {
        oss << messages[i];
        if (i < messages.size() - 1)
            oss << ",";
    }
    oss << "]}";
    std::string body = oss.str();
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        ("Authorization: Bearer " + m_token)
    };

    try {
        std::string url = "https://api.openai.com/v1/chat/completions";
        std::cout << "Body content: " << body << std::endl;
        std::string response = httpService->MakeRequest(url, HttpService::ResponseMethod::POST, body, headers);
        nlohmann::json jsonResponse = nlohmann::json::parse(response);
        std::cout << "Response content: " << response << std::endl;
        std::string message = jsonResponse["choices"][0]["message"]["content"].get<std::string>();
        m_chatHistory[from].push_back(std::make_pair("assistant", message));

        return message;
    } catch (const std::exception& e) {
        std::cout << "Error: GptSerivce::Prompt(): Error sending HTTP request" << std::endl;
        std::cout << body << std::endl;
        m_chatHistory[from].pop_back();
        return std::nullopt;
    }
}

std::optional<std::string> GptService::CleanPrompt(std::string request, std::vector<std::pair<std::string, std::string>>& chatHistory) {
    request.erase(std::remove_if(request.begin(), request.end(), [](char c){
        return c == '`' || c == '"' || c == '\'';
    }), request.end());

    std::string processedRequest = processString(request);

    std::vector<std::string> messages;
    chatHistory.push_back(std::make_pair("user", processedRequest));
    for (auto& [role, message] : chatHistory) {
        std::string processedMessage = processString(message);
        messages.push_back(
            "{\"role\" : \"" + role + "\", \"content\": \"" + processedMessage + "\"}"
        );
    }

    std::ostringstream oss;
    oss << "{\"model\": \"gpt-3.5-turbo\", \"messages\" : [";
    for (size_t i = 0; i < messages.size(); ++i) {
        oss << messages[i];
        if (i < messages.size() - 1)
            oss << ",";
    }
    oss << "]}";
    std::string body = oss.str();
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        ("Authorization: Bearer " + m_token)
    };

    try {
        std::string url = "https://api.openai.com/v1/chat/completions";
        std::string response = httpService->MakeRequest(url, HttpService::ResponseMethod::POST, body, headers);
        nlohmann::json jsonResponse = nlohmann::json::parse(response);
        std::string message = jsonResponse["choices"][0]["message"]["content"].get<std::string>();
        chatHistory.push_back(std::make_pair("assistant", message));

        return message;
    } catch (const std::exception& e) {
        std::cout << "Error: GptSerivce::CleanPrompt(): Error sending HTTP request" << std::endl;
        std::cout << body << std::endl;
        chatHistory.pop_back();
        return std::nullopt;
    }
}

std::string GptService::WriteArticle(std::string articleTheme) {
    std::string message = ("Напиши статью на тему " + articleTheme + " на 2000 символов без заключения.");
    std::vector<std::pair<std::string, std::string>> chatHistory;
    std::string result;

    std::optional<std::string> gptResponse = CleanPrompt(message, chatHistory);
    if (!gptResponse.has_value()) {
        std::cout << "Error: GptService::WriteArticle(): Error on generating article!" << std::endl;
        return "error";
    }

    result = gptResponse.value();

    std::string finish = "Допиши статью еще на 2000 символов без заключения";

    gptResponse = CleanPrompt(finish, chatHistory);
    if (!gptResponse.has_value()) {
        std::cout << "Error: GptService::WriteArticle(): Error on writing article!" << std::endl;
        return "error";
    }

    result += gptResponse.value();

    finish = "Допиши статью еще на 2000 символов теперь с заключением";

    gptResponse = CleanPrompt(finish, chatHistory);
    if (!gptResponse.has_value()) {
        std::cout << "Error: GptService::WriteArticle(): Error on finishing article!" << std::endl;
        return "error";
    }

    result += gptResponse.value();

    std::cout << result << std::endl;

    return result;
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

std::string GptService::processString(const std::string& s) {
    std::string escaped;
    for (char c : s) {
        switch (c) {
            case '"':
            escaped += "\\\"";
            break;
            case '\\':
            escaped += "\\\\";
            break;
            case '\b':
            escaped += "\\b";
            break;
            case '\f':
            escaped += "\\f";
            break;
            case '\n':
            escaped += "\\n";
            break;
            case '\r':
            escaped += "\\r";
            break;
            case '\t':
            escaped += "\\t";
            break;
            default:
            escaped += c;
            break;
        }
    }
    return escaped;
}
    
}
