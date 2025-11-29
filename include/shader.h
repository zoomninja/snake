#ifndef SHADER_H   /* Include guard */
#define SHADER_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"


GLuint shaderProgram;
GLuint lightShaderProgram;

const char* loadShaderSource(const char* filePath);
void setUniform(GLuint shaderProgram, const char* uniformName, const mat4 matrix);
void setUniformVec3(GLuint shaderProgram, const char* uniformName, const vec3 vector);
void setUniformFloat(GLuint shaderProgram, const char* uniformName, const float num);
void setUniformInt(GLuint shaderProgram, const char* uniformName, const int num);
void loadShaders();

#endif // SHADER_H
