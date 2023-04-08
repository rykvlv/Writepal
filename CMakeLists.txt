cmake_minimum_required(VERSION 3.10.2)
project(Writepal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)

include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

set(SOURCES include/Bot/Bot.h 
            include/Service/HttpService.h
            include/Service/GptService.h
            include/System/EventManager/EventManager.hpp
            include/System/EventManager/Events/PromptEvent.hpp
            include/System/EventManager/Events/ArticleEvent.hpp
            3rdparty/include/nlohmann/json.hpp
            src/Bot/Bot.cpp src/main.cpp 
            src/Service/HttpService.cpp
            src/Service/GptService.cpp
            src/System/EventManager/EventManager.cpp
            )
add_executable(Writepal ${SOURCES})
target_include_directories(Writepal PUBLIC include/ 3rdparty/include/)
target_link_libraries(Writepal /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})