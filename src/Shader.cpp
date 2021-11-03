#include "Shader.h"
#include <fstream>
#include <istream>
#include <sstream>
#include <spdlog/spdlog.h>


GLuint create_shader_program(const char *vertex_shader_source, const char *fragment_shader_source)
{
    // 生成并编译着色器
    // 顶点着色器
    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    int success;
    char info_log[512];
    // 检查着色器是否成功编译，如果编译失败，打印错误信息
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        spdlog::error("[OpenGL] {}", info_log);
    }
    // 片段着色器
    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    // 检查着色器是否成功编译，如果编译失败，打印错误信息
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        spdlog::error("[OpenGL] {}", info_log);
    }
    // 链接顶点和片段着色器至一个着色器程序
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // 检查着色器是否成功链接，如果链接失败，打印错误信息
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        spdlog::error("[OpenGL] {}", info_log);
    }
    // 删除着色器
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
