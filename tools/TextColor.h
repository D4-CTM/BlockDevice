#ifndef __TEXT_COLORS__
#define __TEXT_COLORS__

//Inspired mainly on the code provided on our last proyecy (it can be found on the connect4 repo)
namespace AnsiCodes {
    static constexpr const char* DEFAULT = "\x1B[0m";
    static constexpr const char* BLACK = "\x1B[30m";
    static constexpr const char* RED = "\x1B[31m";
    static constexpr const char* GREEN = "\x1B[32m";
    static constexpr const char* YELLOW = "\x1B[33m";
    static constexpr const char* BLUE = "\x1B[34m";
    static constexpr const char* MAGENTA = "\x1B[35m";
    static constexpr const char* CYAN = "\x1B[36m";
    static constexpr const char* WHITE = "\x1B[37m";

    static constexpr const char* BRIGHT_BLACK = "\x1B[90m";
    static constexpr const char* BRIGHT_RED = "\x1B[91m";
    static constexpr const char* BRIGHT_GREEN = "\x1B[92m";
    static constexpr const char* BRIGHT_YELLOW = "\x1B[93m";
    static constexpr const char* BRIGHT_BLUE = "\x1B[94m";
    static constexpr const char* BRIGHT_MAGENTA = "\x1B[95m";
    static constexpr const char* BRIGHT_CYAN = "\x1B[96m";
    static constexpr const char* BRIGHT_WHITE = "\x1B[97m";
};

#endif //__TEXT_COLOR__