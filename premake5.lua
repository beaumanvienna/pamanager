workspace "pulseAudioTesting"
    architecture "x86_64"
    configurations 
    {
        "Debug",
        "Release"
    }

project "pa"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    defines
    {
        "_REENTRANT"
    }

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
    }

    includedirs 
    { 
        "src",
    }

    links
    {
        "pulse",
        "pthread"
    }

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
        kind "ConsoleApp"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"
        kind "ConsoleApp"
