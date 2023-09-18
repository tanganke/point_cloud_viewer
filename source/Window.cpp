#include "Window.h"
#include "PointCloud.h"
#include "Shader.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

extern PointCloud point_cloud;

double mouse_scroll_state[2];
void   scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  spdlog::debug("Scrolling: {}, {}", xoffset, yoffset);
  mouse_scroll_state[0] = xoffset;
  mouse_scroll_state[1] = yoffset;
}

static const char* vertexShader = R"(#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;

out vec3 fColor;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    if(model[2][2] > 0.5)
        fColor = color;
    else 
        fColor = color.xzy;
}
    )";

static const char* fragmentShader = R"(#version 460 core

layout(location = 0) out vec4 FragColor;
in vec3 fColor;

void main()
{
    FragColor = vec4(fColor,1.0);
}
    )";

void Window::InitGLFW() {
  if (!glfwInit()) {
    spdlog::critical("Fail to initialize GLFW library");
    exit(EXIT_FAILURE);
  }
  glfwSetErrorCallback([](int error_code, const char* description) -> void { spdlog::error("[GLFW] error code: {}, description: {}", error_code, description); });
}

void Window::CreateGLFWWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);
  if (!window) {
    spdlog::critical("Failed to create GLFW window");
    exit(EXIT_FAILURE);
  }
}

void Window::InitGL() {
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    spdlog::critical("Failed to initialize OpenGL extensions loader (glad)");
    exit(EXIT_FAILURE);
  }
#ifndef NDEBUG
  spdlog::debug("Enable OpenGL Debug Message Callback");
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
                                   switch (severity)
                                   {
                                   case GL_DEBUG_SEVERITY_HIGH:
                                       spdlog::error("[OpenGL Debug Message] {}", message);
                                       break;
                                   case GL_DEBUG_SEVERITY_MEDIUM:
                                       spdlog::warn("[OpenGL Debug Message] {}", message);
                                       break;
                                   case GL_DEBUG_SEVERITY_LOW:
                                       spdlog::info("[OpenGL Debug Message] {}", message);
                                       break;
                                   default:
                                       spdlog::debug("[OpenGL Debug Message] {}", message);
                                       break;
                                   } },
                         nullptr);
#endif
}

void Window::Run() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.w);
  glPointSize(point_size);

  glfwSetScrollCallback(window, &scroll_callback);

  if (camera_distance < 0) {
    auto bbox       = point_cloud.get_AABB();
    camera_distance = glm::l2Norm(std::get<0>(bbox) - std::get<1>(bbox));
    spdlog::debug("Camera distance: {}", camera_distance);
  }

  // ----------------------------- compile shaders -----------------------------
  GLuint pointCloudShader = create_shader_program(vertexShader, fragmentShader);

  // ----------------------------- buufer data -----------------------------

  GLuint pointCloudVAO;
  GLuint pointCloudVBO[2]; // position, color

  glGenVertexArrays(1, &pointCloudVAO);
  glGenBuffers(2, pointCloudVBO);

  glBindVertexArray(pointCloudVAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, pointCloudVBO[0]);
  glBufferData(GL_ARRAY_BUFFER, point_cloud.get_points().size() * sizeof(glm::vec3), &point_cloud.get_points()[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindBuffer(GL_ARRAY_BUFFER, pointCloudVBO[1]);
  glBufferData(GL_ARRAY_BUFFER, point_cloud.get_colors().size() * sizeof(glm::vec3), &point_cloud.get_colors()[0], GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  double current_frame_time = glfwGetTime();
  double last_frame_time    = current_frame_time;
  double FPS                = 0;
  size_t frame_count        = 0;
  // ----------------------------- main loop -----------------------------
  while (!glfwWindowShouldClose(window)) {
    last_frame_time    = current_frame_time;
    current_frame_time = glfwGetTime();
    frame_count++;
    if (frame_count % 30 == 0)
      FPS = 1 / (current_frame_time - last_frame_time);

    glfwGetWindowSize(window, &width, &height);
    glfwGetFramebufferSize(window, &framebufferSize[0], &framebufferSize[1]);
    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GLFW_TRUE);

    // ------------------------------- window update -------------------------------
    glViewport(0, 0, framebufferSize[0], framebufferSize[1]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // -------------------------------- scene update --------------------------------
    glViewport(scene_windowPos[0], scene_windowPos[1], scene_windowSize[0], scene_windowSize[1]);
    glUseProgram(pointCloudShader);

    static auto      bbox       = point_cloud.get_AABB();
    static auto      center     = glm::vec3(std::get<0>(bbox) + std::get<1>(bbox)) / 2.0f;
    static glm::mat4 model      = glm::mat4(1.0f);
    float&&          camera_x   = camera_distance * sin(camera_phi) * cos(camera_theta);
    float&&          camera_y   = camera_distance * sin(camera_phi) * sin(camera_theta);
    float&&          camera_z   = camera_distance * cos(camera_phi);
    glm::vec3        camera_pos = glm::vec3(camera_x, camera_y, camera_z) + center;
    glm::mat4        view       = glm::lookAt(camera_pos, center, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4        projection = glm::perspective(glm::radians(camera_fov), static_cast<float>(scene_windowSize[0]) / static_cast<float>(scene_windowSize[1]), 0.1f, camera_distance * 2.0f);
    glm::mat4        mvp        = projection * view * model;
    shader_set_uniform(pointCloudShader, "model", model);
    shader_set_uniform(pointCloudShader, "view", view);
    shader_set_uniform(pointCloudShader, "projection", projection);
    shader_set_uniform(pointCloudShader, "mvp", mvp);

    glBindVertexArray(pointCloudVAO);
    glDrawArrays(GL_POINTS, 0, point_cloud.get_points().size());

    // -------------------------------- UI update  ----------------------------------
    BeginUIFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2 { -1, (float)framebufferSize[1] }, ImGuiCond_Always);
    if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar)) {
      ImGui::Text("FPS: %d\n", (int)FPS);
      ImGui::Separator();

      auto               current_window_size = ImGui::GetWindowSize();
      static std::string bbox_min_str        = glm::to_string(std::get<0>(bbox));
      static std::string bbox_max_str        = glm::to_string(std::get<1>(bbox));
      ImGui::Text("Lower Bounding Box:");
      ImGui::Text("\t%.2f, %.2f, %.2f", std::get<0>(bbox).x, std::get<0>(bbox).y, std::get<0>(bbox).z);
      ImGui::Text("Upper Bounding Box:");
      ImGui::Text("\t%.2f, %.2f, %.2f", std::get<1>(bbox).x, std::get<1>(bbox).y, std::get<1>(bbox).z);
      ImGui::Text("Center: %.2f, %.2f, %.2f", center.x, center.y, center.z);
      ImGui::Text("Camera Distance: %f", camera_distance);

      ImGui::Separator(); // --------------------------------------------------
      if (ImGui::SliderFloat("Point Size", &point_size, 0.1f, 20.0f)) {
        glPointSize(point_size);
      }
      if (ImGui::Button("Flip YZ")) {
        if (flip_yz == false) {
          model[1][1] = 0;
          model[1][2] = 1;
          model[2][1] = 1;
          model[2][2] = 0;
          flip_yz     = true;
        } else {
          model[1][1] = 1;
          model[1][2] = 0;
          model[2][1] = 0;
          model[2][2] = 1;
          flip_yz     = false;
        }

        std::get<0>(bbox) = glm::mat3(model) * point_cloud.get_bbox_min();
        std::get<1>(bbox) = glm::mat3(model) * point_cloud.get_bbox_max();
        center            = glm::vec3(std::get<0>(bbox) + std::get<1>(bbox)) / 2.0f;
      }
      if (flip_yz) {
        ImGui::SameLine();
        ImGui::Text("flipped");
      }
      static std::string point_window = fmt::format("Points({})", point_cloud.points.size());
      if (ImGui::CollapsingHeader(point_window.c_str())) {
        ImGui::Text("X,Y,Z");
        const auto& points = point_cloud.get_points();
        for (int i = 0; i < std::min<size_t>(points.size(), 10); i++) {
          std::string str;

          if (flip_yz)
            str = fmt::format("{},{},{}", points[i].x, points[i].z, points[i].y);
          else
            str = fmt::format("{},{},{}", points[i].x, points[i].y, points[i].z);
          ImGui::Text(str.c_str());
        }
        if (points.size() > 10) {
          std::string&& str = fmt::format("...and {} more", points.size() - 10);
          ImGui::Text(str.c_str());
        }
      }

      static std::string color_window = fmt::format("Colors({})", point_cloud.colors.size());
      if (ImGui::CollapsingHeader(color_window.c_str())) {
        ImGui::Text("R,G,B");
        const auto& colors = point_cloud.get_colors();
        for (int i = 0; i < std::min<size_t>(colors.size(), 10); i++) {
          std::string str;
          if (flip_yz)
            str = fmt::format("{},{},{}", colors[i].x, colors[i].z, colors[i].y);
          else
            str = fmt::format("{},{},{}", colors[i].x, colors[i].y, colors[i].z);
          ImGui::Text(str.c_str());
        }
        if (colors.size() > 10) {
          std::string&& str = fmt::format("...and {} more", colors.size() - 10);
          ImGui::Text(str.c_str());
        }
      }

      ImGui::Separator(); // --------------------------------------------------
      if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat("Fov", &camera_fov, 1.0f, 1.0f, 179.0f);
        ImGui::DragFloat("Distance", &camera_distance);
        ImGui::DragFloat("Theta", &camera_theta, 0.1f);
        ImGui::DragFloat("Phi", &camera_phi, 0.1f, 0.01, M_PI_2 - 0.01);
      }

      ImGui::Separator(); // --------------------------------------------------
      if (ImGui::CollapsingHeader("Scene Matrices")) {
        ImGui::Text("Model Matrix:");
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", model[0][0], model[1][0], model[2][0], model[3][0]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", model[0][1], model[1][1], model[2][1], model[3][1]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", model[0][2], model[1][2], model[2][2], model[3][2]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", model[0][3], model[1][3], model[2][3], model[3][3]);
        ImGui::Text("View Matrix:");
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", view[0][0], view[1][0], view[2][0], view[3][0]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", view[0][1], view[1][1], view[2][1], view[3][1]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", view[0][2], view[1][2], view[2][2], view[3][2]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", view[0][3], view[1][3], view[2][3], view[3][3]);
        ImGui::Text("Projection Matrix: ");
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", projection[0][0], projection[1][0], projection[2][0], projection[3][0]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", projection[0][1], projection[1][1], projection[2][1], projection[3][1]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", projection[0][2], projection[1][2], projection[2][2], projection[3][2]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", projection[0][3], projection[1][3], projection[2][3], projection[3][3]);
        ImGui::Text("MVP:");
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", mvp[0][0], mvp[1][0], mvp[2][0], mvp[3][0]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", mvp[0][1], mvp[1][1], mvp[2][1], mvp[3][1]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", mvp[0][2], mvp[1][2], mvp[2][2], mvp[3][2]);
        ImGui::Text("\t%.2f\t%.2f\t%.2f\t%.2f", mvp[0][3], mvp[1][3], mvp[2][3], mvp[3][3]);
      }

      ImGui::Separator(); // --------------------------------------------------
      if (ImGui::CollapsingHeader("Window Setttings"))
        if (ImGui::ColorPicker4("Background Color", &clearColor.r)) {
          glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.w);
        }
    }
    auto last_window_size = ImGui::GetWindowSize();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(last_window_size.x, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2 { (float)framebufferSize[0] - last_window_size.x, (float)framebufferSize[1] }, ImGuiCond_Always);
    if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground)) {
      auto window_size             = ImGui::GetWindowSize();
      auto window_pos              = ImGui::GetWindowPos();
      scene_windowPos[0]           = (int)window_pos.x;
      scene_windowPos[1]           = (int)window_pos.y;
      scene_windowSize[0]          = (int)window_size.x;
      scene_windowSize[1]          = (int)window_size.y;
      static bool       mouse_down = false;
      static glm::dvec2 current_cursor, last_cursor;
      if (ImGui::IsWindowHovered()) {
        scene_windowHovered = true;
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
          if (!mouse_down) {
            glfwGetCursorPos(window, &current_cursor.x, &current_cursor.y);
            last_cursor = current_cursor;
            mouse_down  = true;
            spdlog::debug("mouse down on scene window.");
          } else {
            last_cursor = current_cursor;
            glfwGetCursorPos(window, &current_cursor.x, &current_cursor.y);
            float dx = current_cursor.x - last_cursor.x;
            float dy = current_cursor.y - last_cursor.y;

            camera_theta -= 0.001f * dx;
            float dphi = -0.001f * dy;
            if (dphi < 0) {
              if (camera_phi > glm::radians(10.0f))
                camera_phi += dphi;
            } else if (dphi > 0)
              if (camera_phi < glm::radians(170.0f))
                camera_phi += dphi;
          }
        } else {
          scene_windowHovered = false;
          mouse_down          = false;
        }

        static float d = glm::distance(std::get<0>(bbox), std::get<1>(bbox));
        if (mouse_scroll_state[1] > 0.5) {
          camera_distance       = std::max(d * 0.1f, camera_distance - 0.03f * d);
          mouse_scroll_state[1] = 0;
        } else if (mouse_scroll_state[1] < -0.5) {
          camera_distance += 0.03f * d;
          mouse_scroll_state[1] = 0;
        }
      }
    }
    ImGui::End();

    EndUIFrame();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
}