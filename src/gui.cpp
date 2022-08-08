#include "pch.h"
// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


#include "app.hpp"
#include "spatial/sdf.hpp"

void APP::gui_init( GLFWwindow *window  ){
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    auto style = ImGui::GetStyle();
    ImVec4 c = style.Colors[ImGuiCol_WindowBg];
    c.w = 0.75f;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, c); // Set window background alpha


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init( "#version 100" );
}

float   geom_scale = 100.f;
int     geom_count = 500;
int     geom_type = 0;

void APP::gui_draw(const glm::vec4& clear_color) {
        // Our state
        static bool show_demo_window = false;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {

            ImGui::Begin( "Panel", 0 );
            // Visibility of model
            if (ImGui::CollapsingHeader("Visibility", ImGuiTreeNodeFlags_DefaultOpen ))
            {
                static bool shows[5];
                ImGui::Checkbox("Grid", &show_grids);
                ImGui::Checkbox("Polygon", &show_polygon );
                ImGui::Checkbox("Coloured Model", &show_color );

                ImGui::Checkbox("SDF", &show_iso );
                ImGui::Checkbox("RT Approximation", &show_sdf);
            }
            // Camera Status
            if (ImGui::CollapsingHeader("Cemera", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::LabelText( "Orbit", "%2.0f %2.0f" , camera._orbit.x, camera._orbit.y );
                ImGui::LabelText( "Target", "%2.0f %2.0f %2.0f", camera._target.x, camera._target.y ,camera._target.z );
                ImGui::LabelText( "Zoom", "%2.0f", camera._zoom  );
            }
            // Debug Info
            if (ImGui::CollapsingHeader("SDF status", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::LabelText( "Status", "%s", sdf_progress );
                ImGui::LabelText( "Progress", "%0.2f%%", this->sdf._progress * 100.f );
                ImGui::LabelText( "Duration", "%0.2fs", this->sdf._duration );
                ImGui::LabelText( "Reslution","%d %d %d", this->sdf._grids.x,this->sdf._grids.y, this->sdf._grids.z );
                ImGui::LabelText( "Triangles", "%d", triagles );
            }
            ImGui::Text("Perf: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void APP::gui_quit() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}