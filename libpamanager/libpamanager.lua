
project "libpamanager"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    buildoptions { "-fdiagnostics-color=always -Wall -Wextra -Wno-unused-parameter" }

    defines
    {
        "_REENTRANT",
        "LIBPAMANAGER_VERSION=\"0.1.0\"",
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

    filter { "configurations:Debug" }
        defines { "DEBUG", "VERBOSE" }
        symbols "On"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "On"
