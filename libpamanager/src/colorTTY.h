/* Engine Copyright (c) 2021 Engine Development Team
   https://github.com/beaumanvienna/gfxRenderEngine

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation files
   (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include <iostream>

namespace LibPAmanager
{
    namespace Color
    {

        enum Code
        {
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

        class Modifier
        {
        private:
            Code code;

        public:
            Modifier(Code pCode) : code(pCode) {}

            friend std::ostream& operator<<(std::ostream& os, const Modifier& mod)
            {
                return os << "\033[" << mod.code << "m";
            }
        };
    }
}
//
//logging in all configurations
//
#define PRINT_ERROR(...) fprintf (stderr, "%s\n", __VA_ARGS__)
#define PrintMessage(code, str) LibPAmanager::PrintMessageInternal(code, str)
namespace LibPAmanager
{
    void PrintMessageInternal(Color::Code code, const std::string& x);
}

//
//logging only in debug configuration
//
#ifdef VERBOSE

    #define LOG_INFO(str)     LibPAmanager::LogInfo(str)
    #define LOG_WARN(str)     LibPAmanager::LogWarn(str)
    #define LOG_CRITICAL(str) LibPAmanager::LogCritical(str)
    #define LOG_TRACE(str)    LibPAmanager::LogTrace(str)

    namespace LibPAmanager
    {
        void LogInfo(const std::string& str);
        void LogWarn(const std::string& str);
        void LogCritical(const std::string& str);
        void LogTrace(const std::string& str);
    }

    #define LOG_MESSAGE printf
#else
    #define LOG_INFO(str)
    #define LOG_WARN(str)
    #define LOG_CRITICAL(str)
    #define LOG_TRACE(str)

    #define LOG_MESSAGE(...)
#endif
