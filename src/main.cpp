
#include <Bot/Bot.h>

#include <Service/GptService.h>

int main() {
    std::string main_token = "6214697826:AAHH1mZjbCqABsv__xwOd7y_OCx12uPclYU";
    rykvlv::Bot bot(main_token);
    bot.SetupEvents();
    bot.RegisterEvents();
    while (true) {
        bot.UpdatePolling();
    }

    return 0;
}