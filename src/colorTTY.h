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

void LOG_INFO(const std::string& x)
{
    std::cout << Color::Modifier(Color::FG_GREEN) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
void LOG_WARN(const std::string& x)
{
    std::cout << Color::Modifier(Color::FG_RED) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
void LOG_CRITICAL(const std::string& x)
{
    std::cout << Color::Modifier(Color::FG_YELLOW) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
void LOG_TRACE(const std::string& x)
{
    std::cout << Color::Modifier(Color::FG_BLUE) << x << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}

void LOG_INFO(const std::string& x, int y)
{
    std::cout << Color::Modifier(Color::FG_GREEN) 
              << x << ", " << y
              << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
void LOG_WARN(const std::string& x, int y)
{
    std::cout << Color::Modifier(Color::FG_RED) 
              << x << ", " << y
              << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
void LOG_CRITICAL(const std::string& x, int y)
{
    std::cout << Color::Modifier(Color::FG_YELLOW)
              << x << ", " << y
              << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
void LOG_TRACE(const std::string& x, int y)
{
    std::cout << Color::Modifier(Color::FG_BLUE)
              << x << ", " << y
              << Color::Modifier(Color::FG_DEFAULT) << std::endl;
}
