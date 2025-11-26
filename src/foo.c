#include "../include/shader.h"
#include "../include/cglm/cglm.h"
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"



const char* loadShaderSource(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open shader file '%s'\n", filePath);
        return NULL;
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the shader source + null terminator
    char* shaderSource = (char*)malloc(fileSize + 1);
    if (shaderSource == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for shader source\n");
        fclose(file);
        return NULL;
    }

    // Read file content
    fread(shaderSource, 1, fileSize, file);
    shaderSource[fileSize] = '\0'; // Null-terminate the string

    fclose(file);
    return shaderSource;
}

void setUniform(GLuint program, const char* uniformName, const mat4 matrix){
	unsigned int uniformLoc = glGetUniformLocation(program, uniformName); //send matrix to the shader thru uniforms
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, (float *)matrix);
}

void setUniformVec3(GLuint program, const char* uniformName, const vec3 vector){
	unsigned int uniformLoc = glGetUniformLocation(program, uniformName); //send vec3 to the shader thru uniforms
	glUniform3fv(uniformLoc, 1, vector);
}

void setUniformFloat(GLuint program, const char* uniformName, const float num){
	unsigned int uniformLoc = glGetUniformLocation(program, uniformName); //send float to the shader thru uniforms
	glUniform1f(uniformLoc, num);
}

void setUniformInt(GLuint program, const char* uniformName, const int num){
	unsigned int uniformLoc = glGetUniformLocation(program, uniformName); //send int to the shader thru uniforms
	glUniform1i(uniformLoc, num);
}

void loadShaders(){
    const char *vertexSource = loadShaderSource("src/shaders/shader.vs");
    const char *fragmentSource = loadShaderSource("src/shaders/shader.fs");
	
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //create the shader into a unsigned int
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //fragment shader
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram(); //makes a shader program

    glAttachShader(shaderProgram, vertexShader); //attaches the vertexShader to the shaderProgram
    glAttachShader(shaderProgram, fragmentShader); //same but with fragmentShader

    glLinkProgram(shaderProgram); //links the shader program or whatever

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); //check for errors
    if(!success) {
    	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }

    glDeleteShader(fragmentShader);



    fragmentSource = loadShaderSource("src/shaders/lightShader.fs");


    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //fragment shader
    
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    lightShaderProgram = glCreateProgram(); //makes a shader program

    glAttachShader(lightShaderProgram, vertexShader); //attaches the vertexShader to the shaderProgram
    glAttachShader(lightShaderProgram, fragmentShader); //same but with fragmentShader

    glLinkProgram(lightShaderProgram); //links the shader program or whatever



    glGetProgramiv(lightShaderProgram, GL_LINK_STATUS, &success); //check for errors
    if(!success) {
    	glGetProgramInfoLog(lightShaderProgram, 512, NULL, infoLog);
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}


