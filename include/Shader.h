#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <glm/glm.hpp>

/**
 * @brief create shader from GLSL source code
 * @param vertex_shader_source GLSL source code for vertex shader
 * @param fragment_shader_source GLSL source code for fragment shader
 */
GLuint create_shader_program(const char *vertex_shader_source, const char *fragment_shader_source);

/** @brief set shader program's uniform variable */
inline void shader_set_uniform(GLuint program, const char *name, const glm::vec3 &vec)
{
    auto loc = glGetUniformLocation(program, name);
    glUniform3fv(loc, 1, &vec[0]);
}

inline void shader_set_uniform(GLuint program, const char *name, const glm::vec4 &vec)
{
    auto loc = glGetUniformLocation(program, name);
    glUniform4fv(loc, 1, &vec[0]);
}

inline void shader_set_uniform(GLuint program, const char *name, float f)
{
    auto loc = glGetUniformLocation(program, name);
    glUniform1f(loc, f);
}

inline void shader_set_uniform(GLuint program, const char *name, int i)
{
    auto loc = glGetUniformLocation(program, name);
    glUniform1i(loc, i);
}

inline void shader_set_uniform(GLuint program, const char *name, const glm::mat3 &mat)
{
    auto loc = glGetUniformLocation(program, name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
}

inline void shader_set_uniform(GLuint program, const char *name, const glm::mat4 &mat)
{
    auto loc = glGetUniformLocation(program, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}

inline void shader_set_uniform(GLuint program, const char *name, bool value)
{
    auto loc = glGetUniformLocation(program, name);

    if (value)
        glUniform1i(loc, 0xFF);
    else
        glUniform1i(loc, 0);
}

inline void shader_set_uniform(GLuint program, const char *name, GLuint value)
{
    auto loc = glGetUniformLocation(program, name);
    glUniform1ui(loc, value);
}
