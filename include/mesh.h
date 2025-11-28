#ifndef MESH_H   /* Include guard */
#define MESH_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include "assimp/cimport.h" // For the C API functions
#include "assimp/scene.h"   // For aiScene, aiMesh, etc.
#include "assimp/postprocess.h" // For aiProcess_... flags

typedef struct {
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    unsigned int index_count;
} Mesh;

typedef struct {
    Mesh* meshes;
    unsigned int mesh_count;
} Model;

Model loadModel(const char* path);
static Mesh loadMesh(const struct aiMesh* mesh);
void drawModel(const Model* model);
void freeModel(Model* model);


#endif // MESH_H