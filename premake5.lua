workspace "WeatherAPI"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "WeatherAPI"

project "WeatherAPI"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    -- Source files
    files { "src/**.cpp", "src/**.h" }

    -- Local include and library directories
    includedirs { "include" }
    libdirs { "libs" }

    -- Link only libcurl
    links { "libcurl" }

    filter "system:windows"
        systemversion "latest"

        -- Copy the required DLLs after build
        postbuildcommands {
            '{COPY} "%{wks.location}/libs/libcurl.dll" "%{cfg.targetdir}"',
            '{COPY} "%{wks.location}/libs/zlib1.dll" "%{cfg.targetdir}"'
        }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
