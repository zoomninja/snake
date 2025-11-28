#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/mesh.h"
#include "../include/shader.h"
#include "../include/cglm/cglm.h"
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/stb/stb_image.h"
#include "../include/assimp/cimport.h" // For the C API functions
#include "../include/assimp/scene.h"   // For aiScene, aiMesh, etc.
#include "../include/assimp/postprocess.h" // For aiProcess_... flags






Model loadModel(const char* path){
    Model model = {0};

    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        printf("assimp error: %s\n", aiGetErrorString());
        return model;
    }
    printf("Number of materials: %d\n", scene->mNumMaterials);

    model.mesh_count = scene->mNumMeshes;
    model.meshes = malloc(sizeof(Mesh) * model.mesh_count);
    printf("Number of meshes: %d\n", scene->mNumMeshes);
    
    
    for (unsigned int i = 0; i < model.mesh_count; i++){
        const struct aiMesh* ai_mesh = scene->mMeshes[i];
        model.meshes[i] = loadMesh(ai_mesh);
    }

    aiReleaseImport(scene);
    return model;
}

static Mesh loadMesh(const struct aiMesh* mesh){
    Mesh m = {0};

    bool has_tex = (mesh->mTextureCoords[0] != NULL);
    unsigned int vertex_size = 8;



    float* vertices = malloc(sizeof(float) * mesh->mNumVertices * vertex_size);
    unsigned int* indices = malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);

    

    //vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++){
        float* v = &vertices[i * vertex_size];

        //position
        v[0] = mesh->mVertices[i].x;
        v[1] = mesh->mVertices[i].y;
        v[2] = mesh->mVertices[i].z;

        //normal
        v[3] = mesh->mNormals[i].x;
        v[4] = mesh->mNormals[i].y;
        v[5] = mesh->mNormals[i].z;

        if (has_tex){
            v[6] = mesh->mTextureCoords[0][i].x;
            v[7] = mesh->mTextureCoords[0][i].y;
        }else{
            v[6] = 0.0f;
            v[7] = 0.0f;
        }
    }

    //indices
    unsigned int idx = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++){
        const struct aiFace* f = &mesh->mFaces[i];
        indices[idx++] = f->mIndices[0];
        indices[idx++] = f->mIndices[1];
        indices[idx++] = f->mIndices[2];
    }

    

    m.index_count = idx;

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);
    glGenBuffers(1, &m.EBO);

    glBindVertexArray(m.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->mNumVertices * vertex_size, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, indices, GL_STATIC_DRAW);

    int stride = vertex_size * sizeof(float);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //texcoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    free(vertices);
    free(indices);
    return m;
}

void drawModel(const Model* model){
    for (unsigned int i = 0; i < model->mesh_count; i++){
        const Mesh* m = &model->meshes[i];
        glBindVertexArray(m->VAO);
        glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, 0);
    }
}

void freeModel(Model* model){
    for (unsigned int i = 0; i < model->mesh_count; i++){
        glDeleteVertexArrays(1, &model->meshes[i].VAO);
        glDeleteBuffers(1, &model->meshes[i].VBO);
        glDeleteBuffers(1, &model->meshes[i].EBO);
    }
    free(model->meshes);
}