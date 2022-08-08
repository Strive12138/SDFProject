#include "app.hpp"


void APP::app_init_internal() {
        // Setup window
        #if !defined(NDEBUG)
        glfwSetErrorCallback( [](int error, const char* description) {
            fprintf(stderr, "Glfw Error %d: %s\n", error, description);
        } );
        #endif

        if (!glfwInit())
            return exit(1);
        
        #if !defined(NDEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); 
        #endif
        // GL 4.5 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
        // Create window with graphics context
        window = glfwCreateWindow(1280, 720, "SDF", NULL, NULL);
        if (!window)
            exit(1);

        camera._window = window;

        double mx,my;
        glfwGetCursorPos( window, &mx, &my);
        mouse.mouse.x = float(mx);
        mouse.mouse.y = float(my);
                
        glfwSetWindowUserPointer(window, this);

        glfwSetWindowSizeCallback(window,  [](GLFWwindow* window, int width, int height) {
            //auto app =(APP*) glfwGetWindowUserPointer(window);

        });

        glfwSetScrollCallback(window,  [] (GLFWwindow* window, double xoffset, double yoffset) {
            auto app =(APP*) glfwGetWindowUserPointer(window);

            app->mouse.wheelD.x = (float)xoffset;
            app->mouse.wheelD.y = (float)yoffset;

            //std::cout << "wheelX: " << app->wheelD.x << " wheelY:" << app->wheelD.y << std::endl; 

        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            auto app =(APP*) glfwGetWindowUserPointer(window);

            glm::vec2 mouse = { (float)xpos, (float)ypos };
            app->mouse.mouseD = mouse - app->mouse.mouse;
            app->mouse.mouse = mouse;

        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            auto app =(APP*) glfwGetWindowUserPointer(window);

            bool pressed= action == GLFW_PRESS;

            bool *botton = nullptr;

            if ( button == GLFW_MOUSE_BUTTON_LEFT ) {
                botton = &app->mouse.button_stats[0];
            }            
            if ( button == GLFW_MOUSE_BUTTON_MIDDLE ) {
                botton = &app->mouse.button_stats[1];
            }
            if ( button == GLFW_MOUSE_BUTTON_RIGHT ) {
                botton = &app->mouse.button_stats[2];
            }

            *botton = pressed;

            glfwSetInputMode(window, GLFW_CURSOR,  
                app->mouse.button_stats[1]||app->mouse.button_stats[2] ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        });

	    glfwSetKeyCallback( window,  [](GLFWwindow* aWindow, int aKey, int /*aScanCode*/, int aAction, int /*aModifierFlags*/) {

            if ( GLFW_PRESS == aAction) {
                if (GLFW_KEY_ESCAPE == aKey )
                {
                    glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
                }
            }

        });

        int ww, wh;
        glfwGetFramebufferSize(window, &ww, &wh);
        camera._on_aspect_changed( float(ww)/float(wh) );

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            auto app =(APP*) glfwGetWindowUserPointer(window);
            app->camera._on_aspect_changed( float(width)/float(height) );
        });

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            exit(1);
        }
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback( 
                []( GLenum source,
                        GLenum type,
                        GLuint id,
                        GLenum severity,
                        GLsizei length,
                        const GLchar* message,
                        const void* userParam )
                    {
                        fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                            ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                                type, severity, message );
                    }
            , 0 );
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

            printf( "Note: Debug context initialized\n" );
        }
        glfwSwapInterval(1); // Enable vsync

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);    
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
}


void APP::app_quit_internal() {

        glfwDestroyWindow(window);
        glfwTerminate();    
}

void APP::run() {

        while (!glfwWindowShouldClose(window))
        {
            this->mouse.wheelD = {0,0};
            this->mouse.mouseD = {0,0};
            glfwPollEvents();

            float current = glfwGetTime();
            delta = current - last;
            accum += delta;
            last = current; 

            camera.step( delta, mouse );

            glfwGetFramebufferSize(window, &screen_size.x, &screen_size.y);
            if (screen_size.x!=0 && screen_size.y!=0 ) {
                glViewport(0, 0, screen_size.x, screen_size.y);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                app_draw_scane( delta );
                gui_draw(clear_color);
                glfwSwapBuffers(window);
            }
        }
}

int main(int, char**)
{

    APP app;
    
    app.init();
    app.run();
    app.quit();

    return 0;
}
