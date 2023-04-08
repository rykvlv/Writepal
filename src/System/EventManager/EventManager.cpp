#include <System/EventManager/EventManager.hpp>


namespace rykvlv {

void EventManager::RegisterHandler(const std::string& eventName, std::function<void(const void*)> handler) {
    m_handlers[eventName].push_back(handler);
}

void EventManager::TriggerEvent(const std::string& eventName, const void* eventData) {
    auto handlers = m_handlers.find(eventName);
    if (handlers != m_handlers.end()) {
        for (auto& handler : handlers->second) {
            handler(eventData);
        }
    }
}

}