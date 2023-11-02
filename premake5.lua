workspace "Fabric"
    startproject "tests"

    configurations 
    { 
        "Debug", 
        "Profiling", 
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    OutputDir = "%{wks.location}/build/bin/%{cfg.buildcfg}"
    IntermediateDir = "%{wks.location}/build/obj/%{cfg.buildcfg}"

    filter "configurations:Debug"
        defines
        {
            "FB_DEBUG"
        }
        symbols "On"
    
    filter "configurations:Profiling"
        defines
        {
            "FB_PROFILING"
        }
        optimize "Debug"
    
    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"

group "dependencies"
group ""

group "core"
    include "core"
group ""

group "tools"
    --include "editor"
    include "tests"
group ""