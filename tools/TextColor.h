#ifndef __TEXT_COLORS__
#define __TEXT_COLORS__
#include <iostream>

//Inspired mainly on the code provided on our last proyecy (it can be found on the connect4 repo)
namespace AnsiCodes {
    inline constexpr const char* DEFAULT = "\x1B[0m";
    inline constexpr const char* BLACK = "\x1B[30m";
    inline constexpr const char* RED = "\x1B[31m";
    inline constexpr const char* GREEN = "\x1B[32m";
    inline constexpr const char* YELLOW = "\x1B[33m";
    inline constexpr const char* BLUE = "\x1B[34m";
    inline constexpr const char* MAGENTA = "\x1B[35m";
    inline constexpr const char* CYAN = "\x1B[36m";
    inline constexpr const char* WHITE = "\x1B[37m";

    inline constexpr const char* BRIGHT_BLACK = "\x1B[90m";
    inline constexpr const char* BRIGHT_RED = "\x1B[91m";
    inline constexpr const char* BRIGHT_GREEN = "\x1B[92m";
    inline constexpr const char* BRIGHT_YELLOW = "\x1B[93m";
    inline constexpr const char* BRIGHT_BLUE = "\x1B[94m";
    inline constexpr const char* BRIGHT_MAGENTA = "\x1B[95m";
    inline constexpr const char* BRIGHT_CYAN = "\x1B[96m";
    inline constexpr const char* BRIGHT_WHITE = "\x1B[97m";

    inline const void showRootDirectory(const std::string& path) {
        if (!path.empty()) {
            std::cout << GREEN << "User@Block_Device: ~/" << path;
        }
        std::cout << DEFAULT << "$ ";
    }

    inline const void showWarning(const std::string& warning) {
        if (!warning.empty()) { std::cerr << YELLOW << warning << '\n'; }
        std::cout << DEFAULT;
    }

    inline const void showError(const std::string& error) {
        if (!error.empty()) { std::cerr << RED << error << '\n'; }
        std::cout << DEFAULT;
    }

    inline const void showSuggestion(const std::string& suggestion) {
        if (!suggestion.empty()) { std::cerr << BLUE << suggestion << '\n'; }
        std::cout << DEFAULT;
    }

};

#endif //__TEXT_COLOR__