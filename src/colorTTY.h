#pragma once

#include <iostream>
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}

#define LOG_INFO(x) std::cout << Color::Modifier(Color::FG_GREEN) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
#define LOG_WARN(x) std::cout << Color::Modifier(Color::FG_RED) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
#define LOG_CRITICAL(x) std::cout << Color::Modifier(Color::FG_YELLOW) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
#define LOG_TRACE(x) std::cout << Color::Modifier(Color::FG_BLUE) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
