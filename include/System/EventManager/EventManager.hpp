#pragma once 

#include <string>
#include <map>
#include <vector>
#include <functional>

namespace rykvlv {
class EventManager {
public:
    void RegisterHandler(const std::string& eventName, std::function<void(const void*)> handler);
    void TriggerEvent(const std::string& eventName, const void* eventData);
private:
    std::map<std::string, std::vector<std::function<void(const void*)>>> m_handlers;
};
}