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
    GLuint texture_id; //0 if no texture
} Mesh;

typedef struct {
    char path[512];
    GLuint tex_id;
} Texture;

typedef struct {
    Mesh* meshes;
    unsigned int mesh_count;
} Model;

Texture texture_cache[128];
unsigned int texture_cache_count;

Model loadModel(const char* path);
static Mesh loadMesh(const struct aiMesh* mesh);
GLuint loadMaterialTexture(const struct aiMaterial* mat, const char* modelDir);
GLuint loadTextureFromFile(const char* filename);
void drawModel(GLuint shader, const Model* model);
void freeModel(Model* model);


#endif // MESH_H