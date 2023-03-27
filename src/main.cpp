
#include <Bot/Bot.h>

#include <Service/GptService.h>

int main() {
    rykvlv::Bot bot("6214697826:AAHH1mZjbCqABsv__xwOd7y_OCx12uPclYU");
    bot.SetupEvents();
    bot.StartPolling();
    // rykvlv::GptService gptService("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");
    // rykvlv::GptService::GptResponse gptResponse =  gptService.Promt("Hello, ChatGPT");
    // for (auto choice : gptResponse.choices) {
    //     std::cout << "Message:  " << choice.message.content << std::endl;
    // }
    return 0;
}