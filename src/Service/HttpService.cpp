#include <Service/HttpService.h>

#include <iostream>

namespace rykvlv {

HttpService::HttpService() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpService::~HttpService() {
    
    curl_global_cleanup();
}

std::string HttpService::GetResponse(const std::string& url) {
    curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
    
    return response_data;
}

std::string HttpService::MakeRequest(const std::string& url, ResponseMethod responseMethod, const std::string& body, std::vector<std::string> headers) {
    CURLcode res;
    curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Error initializing CURL" << std::endl;
        return "<error>";
    }

    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    switch (responseMethod) {
        case ResponseMethod::GET: {
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
            break;
        }
        case ResponseMethod::POST: {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            break;
        }
        default: {
            std::cerr << "Invalid method type" << std::endl;
            curl_easy_cleanup(curl);
            return "<error>";
        }
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist *header_list = nullptr;

    for (const auto& header : headers) {
        header_list = curl_slist_append(header_list, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
        return "<error>";
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(header_list);

    return response;
}

unsigned int HttpService::WriteCallback(void* contents, unsigned int size, unsigned int nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

}