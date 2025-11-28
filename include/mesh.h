#ifndef MESH_H   /* Include guard */
#define MESH_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include "assimp/cimport.h" // For the C API functions
#include "assimp/scene.h"   // For aiScene, aiMesh, etc.
#include "assimp/postprocess.h" // For aiProcess_... flags



struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
};

struct Texture {
    unsigned int id;
    char* type;
    char* path;
};

struct Model {
    unsigned int numOfMeshes;
    struct Mesh* meshes;
    unsigned int numOfTextures;
    struct Texture* loadedTextures;
};

struct Mesh{
    unsigned int numOfVertices;
    struct Vertex* vertices;
    unsigned int numOfIndices;
    unsigned int* indices;
    unsigned int numOfTextures;
    struct Texture* textures;
};


void Draw(GLuint shader, struct Mesh mesh);
void Mesh(struct Mesh mesh);
struct Model createModel(char* path);
void drawModel(GLuint shader, struct Model model);
void loadModel(char* path, struct Model model);
void processNode(struct aiNode *node, const struct aiScene *scene, char* path, struct Model model);
struct Mesh processMesh(struct aiMesh *mesh, const struct aiScene *scene, char* path, struct Model model);
struct Texture* loadMaterialTextures(struct aiMaterial *mat, enum aiTextureType type, char* typeName, char* path, struct Model model);
unsigned int TextureFromFile(const char *path, char* directory);



#endif // MESH_H