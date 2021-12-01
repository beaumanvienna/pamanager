workspace "gfxRenderEngine"
    architecture "x86_64"
    configurations 
    {
        "Debug",
        "Release",
    }

project "pa"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    defines
    {
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

    libdirs
    {
    }
