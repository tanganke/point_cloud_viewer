#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <string>
#include <limits.h>

using namespace glm;

/**
 * @brief OpenGL window class
 * @details This class is used to create an OpenGL window and handle events.
 *
 * Usage:
 * ```cpp
 * #include "Window.h"
 *
 * Window window("point cloud viewer", 1600, 1000);
 * try
 * {
 *     window.Run();
 * }
 * catch (const std::exception &e)
 * {
 *     spdlog::critical("{}", e.what());
 *     exit(EXIT_FAILURE);
 * }
 * ```
 */
class Window
{
private:
    const std::string appName;
    int width, height;
    GLFWwindow *window;

    vec4 clearColor{1, 1, 1, 1};
    int framebufferSize[2];

    int scene_windowPos[2]{0, 0};
    int scene_windowSize[2]{800, 800};

    float camera_fov{45.0f};
    float camera_distance{-1.0f};
    float camera_theta{};
    float camera_phi{M_PI_2};

    bool flip_yz{false};
    float point_size = 5.0f;

private:
    void InitGLFW();

    void CreateGLFWWindow();

    void InitGL();

    inline void InitImGui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
        ImGui_ImplOpenGL3_Init();

        // auto font_file = std::string(ASSERTS_DIR) + "Fonts/DroidSans.ttf";
        float xscale, yscale;
        glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xscale, &yscale);
        // io.Fonts->AddFontFromFileTTF(font_file.c_str(), 15 * (xscale + yscale) / 2);
        io.FontGlobalScale = (xscale + yscale) / 2;
    }

    inline void BeginUIFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    inline void EndUIFrame()
    {
        ImGui::Render();
    }

public:
    Window(const char *_appName, int _width, int _height)
        : appName{_appName}, width{_width}, height{_height}
    {
        InitGLFW();
        CreateGLFWWindow();
        InitGL();
        InitImGui();
    };

    ~Window()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Run();

    inline vec4 GetClearColor()
    {
        return clearColor;
    }

    inline void SetClearColor(const vec4 &color)
    {
        clearColor = color;
        glClearColor(color.r, color.g, color.b, color.a);
    }

    inline const std::string &GetApplicationName()
    {
        return appName;
    }
};
