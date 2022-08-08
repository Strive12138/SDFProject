
project(imgui)

include_directories( 
    "${CMAKE_CURRENT_SOURCE_DIR}"
)


add_library( ${PROJECT_NAME}  STATIC
  "imgui.cpp"
  "imgui_draw.cpp"
  "imgui_widgets.cpp"
  "imgui_tables.cpp"
  "imgui_demo.cpp"
  "backends/imgui_impl_glfw.cpp"
  "backends/imgui_impl_opengl3.cpp"
)