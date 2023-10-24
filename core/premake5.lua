project "core"
    architecture "x64"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "%{OutputDir}"
    objdir "%{IntermediateDir}"

    files 
    { 
        "**.hpp", 
        "**.cpp"
    }

    includedirs
    {
        ""
    }