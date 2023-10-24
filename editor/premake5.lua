project "editor"
    kind "WindowedApp"
    language "C#"
    csversion "11"
    dotnetframework "net7.0-windows"
    targetdir "%{OutputDir}"
    objdir "%{IntermediateDir}"

    files 
    { 
        "**.cs", 
        "**.xaml"
    }

    flags
    {
        "WPF"
    }