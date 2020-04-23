-- premake5.lua
workspace "tas"
	architecture "x64"
	configurations 
	{ 
		"Debug",
		"Release"
	}
	
	location "build"

project "tas"
	kind "ConsoleApp"
	language "C"
    targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"
	location "build"

    files
	{
		"src/**.h",
		"src/**.c"
	}
	
	filter "system:windows"
		systemversion "latest"
		
		defines 
		{
			"_CRT_SECURE_NO_WARNINGS",
			"strdup=_strdup"
		}
		
    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
		optimize "On"
