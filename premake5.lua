workspace "SDF"
	language "C++"
	cppdialect "C++17"

	platforms { "x64" }
	configurations { "release", "debug" }

	flags "NoPCH"
	flags "MultiProcessorCompile"

	startproject "sdf"

	debugdir "%{wks.location}"
	objdir "prebuild/%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
	targetsuffix "-%{cfg.buildcfg}-%{cfg.platform}-%{cfg.toolset}"
	
	-- Default toolset options
	filter "toolset:gcc or toolset:clang"
		linkoptions { "-pthread" }
		buildoptions { "-march=native", "-Wall", "-pthread" }

	filter "toolset:msc-*"
		defines { "_CRT_SECURE_NO_WARNINGS=1" }
		defines { "_SCL_SECURE_NO_WARNINGS=1" }
		buildoptions { "/utf-8" }
	
	filter "*"

	-- default libraries
	filter "system:linux"
		links "dl"
	
	filter "system:windows"

	filter "*"

	-- default outputs
	filter "kind:StaticLib"
		targetdir "prebuild/lib/"

	filter "kind:ConsoleApp"
		targetdir "prebuild/bin/"
		targetextension ".exe"
	
	filter "*"

	--configurations
	filter "debug"
		symbols "On"
		defines { "_DEBUG=1" }

	filter "release"
		optimize "On"
		defines { "NDEBUG=1" }

	filter "*"

-- Third party dependencies
include "extern" 



-- Projects
project "sdf"
	includedirs( "src" );

	local sources = { 
		"src/**.cpp",
		"src/**.hpp",
		"src/**.hxx",
		"src/**.h"
	}

	kind "ConsoleApp"
	location "src"

	files( sources )

	links "x-glad"
	links "x-glad"
	links "x-glfw"
	links "x-tinyobj"
	links "x-imgui"
	dependson "x-glm" 
	-- The Visual Studio implementation integrates natively with the system thread pool, 
	-- GCC and Clang needs us to take a dependency on the Intel Thread Building Blocks library.
	filter "system:linux"
		links "tbb"

--EOF
