#pragma once

#include <string>
#include <vector>

#include <curl/curl.h>


namespace rykvlv {
class HttpService {
public:
    enum class ResponseMethod {
        POST,
        GET
    };
public:
    HttpService();
    ~HttpService();

    std::string GetResponse(const std::string& url);
    std::string MakeRequest(const std::string& url, ResponseMethod responseMethod, const std::string& body, std::vector<std::string> headers);

    
private:
    static unsigned int WriteCallback(void* contents, unsigned int size, unsigned int nmemb, void* userp);

    CURL* curl;
};
}
