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

    files 
    { 
        "testApplication/**.h", 
        "testApplication/**.cpp",
    }

    includedirs 
    { 
        "testApplication",
        "lib/src"
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

include "lib/libpamanager.lua"
