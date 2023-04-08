
#include <Bot/Bot.h>

#include <Service/GptService.h>
#include <cstdlib>

void setupEnvVariables() {
    // Read the contents of the .env file
    std::ifstream env_file(".env");
    std::stringstream env_stream;
    env_stream << env_file.rdbuf();
    std::string env_contents = env_stream.str();

    // Parse the contents of the .env file
    std::vector<std::string> env_lines;
    std::stringstream env_stream2(env_contents);
    std::string env_line;
    while (std::getline(env_stream2, env_line)) {
        env_lines.push_back(env_line);
    }

    // Set the environment variables
    for (const auto& env_line : env_lines) {
        auto pos = env_line.find('=');
        if (pos != std::string::npos) {
            std::string env_name = env_line.substr(0, pos);
            std::string env_value = env_line.substr(pos + 1);
            setenv(env_name.c_str(), env_value.c_str(), 1);
        }
    }
}

int main() {
    setupEnvVariables();

    std::string token = std::getenv("TELEGRAM_API_KEY");
    rykvlv::Bot bot(token);
    bot.SetupEvents();
    bot.RegisterEvents();
    while (true) {
        bot.UpdatePolling();
    }

    return 0;
}