project "tests"
    architecture "x64"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "%{OutputDir}"
    objdir "%{IntermediateDir}"

    links
    {
        "core"
    }

    includedirs
    {
        "../core/"
    }

    files 
    { 
        "**.hpp", 
        "**.cpp"
    }