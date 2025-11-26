#ifndef MESH_H   /* Include guard */
#define MESH_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"


struct Vertex;
struct Texture;


void Draw(GLuint shader, struct Texture* textures);
Mesh(struct Vertex* vertices, unsigned int* indices, struct Texture* textures);



#endif // MESH_H