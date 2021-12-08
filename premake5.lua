workspace "pulseAudioTesting"
    architecture "x86_64"
    configurations 
    {
        "Debug",
        "Release"
    }

project "testApplication"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    buildoptions { "-fdiagnostics-color=always -Wall -Wextra -Wno-unused-parameter" }

    files 
    { 
        "testbed/**.h", 
        "testbed/**.cpp",
    }

    includedirs 
    { 
        "testbed",
        "libpamanager/src"
    }

    links
    {
        "libpamanager",
        "pulse",
        "pthread"
    }

    filter { "configurations:Debug" }
        defines { "DEBUG", "VERBOSE" }
        symbols "On"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"

include "libpamanager/libpamanager.lua"
