-- Third party projects

includedirs( "glad/include" );
includedirs( "glfw/include" );
includedirs( "glm" );
includedirs( "tinyobjloader" );
includedirs( "imgui" );

project( "x-glad" )
	kind "StaticLib"

	location "."

	files( "glad/src/glad.c" )


project( "x-glfw" )
	kind "StaticLib"

	location "."

	filter "system:linux"
		defines { "_GLFW_X11=1" }

	filter "system:windows"
		defines { "_GLFW_WIN32=1" }

	filter "*"

	files {
		"glfw/src/context.c",
		"glfw/src/egl_context.c",
		"glfw/src/init.c",
		"glfw/src/input.c",
		"glfw/src/internal.h",
		"glfw/src/mappings.h",
		"glfw/src/monitor.c",
		"glfw/src/null_init.c",
		"glfw/src/null_joystick.c",
		"glfw/src/null_joystick.h",
		"glfw/src/null_monitor.c",
		"glfw/src/null_platform.h",
		"glfw/src/null_window.c",
		"glfw/src/platform.c",
		"glfw/src/platform.h",
		"glfw/src/vulkan.c",
		"glfw/src/window.c",
		"glfw/src/osmesa_context.c"
	};

	filter "system:linux"
		files {
			"glfw/src/posix_*",
			"glfw/src/x11_*", 
			"glfw/src/xkb_*",
			"glfw/src/glx_*",
			"glfw/src/linux_*",
		};
	filter "system:windows"
		files {
			"glfw/src/win32_*",
			"glfw/src/wgl_*", 
		};

	filter "*"




project( "x-glm" )
	kind "Utility"

	location "."

	files( "glm/glm/**.h" )
	files( "glm/glm/**.hpp" )
	files( "glm/glm/**.inl" )

project( "x-tinyobj" )
	kind "StaticLib"

	location "."

	files( "tinyobjloader/tiny_obj_loader.cc" )


project( "x-imgui" )
	kind "StaticLib"

	location "."
	
	files( "imgui/imgui.cpp" )
	files( "imgui/imgui_draw.cpp" )
	files( "imgui/imgui_widgets.cpp" )
	files( "imgui/imgui_tables.cpp" )
	files( "imgui/imgui_demo.cpp" )
	files( "imgui/backends/imgui_impl_glfw.cpp" )
	files( "imgui/backends/imgui_impl_opengl3.cpp" )

--EOF
