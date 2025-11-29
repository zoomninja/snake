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
#include "../include/assimp/material.h"
#include "../include/assimp/postprocess.h" // For aiProcess_... flags

Texture texture_cache[128];
unsigned int texture_cache_count = 0;






Model loadModel(const char* path){
    Model model = {0};

    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        printf("assimp error: %s\n", aiGetErrorString());
        return model;
    }
    printf("Number of materials: %d\n", scene->mNumMaterials);

    char modelDir[512];
    strcpy(modelDir, path);
    for (int i = strlen(modelDir)-1; i >= 0; i--){
        if (modelDir[i] == '/' || modelDir[i] == '\\'){
            modelDir[i] = '\0';
            break;
        }
    }

    model.mesh_count = scene->mNumMeshes;
    model.meshes = malloc(sizeof(Mesh) * model.mesh_count);
    printf("Number of meshes: %d\n", scene->mNumMeshes);
    
    
    for (unsigned int i = 0; i < model.mesh_count; i++){
        const struct aiMesh* ai_mesh = scene->mMeshes[i];


        
        Mesh mesh = loadMesh(ai_mesh);

        if (ai_mesh->mMaterialIndex >= 0){
            const struct aiMaterial* mat = scene->mMaterials[ai_mesh->mMaterialIndex];
            mesh.texture_id = loadMaterialTexture(mat, modelDir);
        }else{
            mesh.texture_id = 0;
        }

        model.meshes[i] = mesh;
    }

    aiReleaseImport(scene);
    return model;
}

static Mesh loadMesh(const struct aiMesh* mesh){
    Mesh m = {0};

    bool has_tex = (mesh->mTextureCoords[0] != NULL);
    unsigned int vertex_size = has_tex ? 8 : 6;



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
            v[7] = 1.0f - mesh->mTextureCoords[0][i].y;
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
    if (has_tex){
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    

    glBindVertexArray(0);
    free(vertices);
    free(indices);
    return m;
}

GLuint loadMaterialTexture(const struct aiMaterial* mat, const char* modelDir){
    if (aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE) == 0){
        return 0;
    }
    struct aiString str;
    if (aiGetMaterialTexture(mat, aiTextureType_DIFFUSE, 0, &str, NULL, NULL, NULL, NULL, NULL, NULL) != AI_SUCCESS){
        return 0;
    }

    //build full path
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", modelDir, str.data);

    for (unsigned int i = 0; i < texture_cache_count; i++){
        if (strcmp(texture_cache[i].path, fullpath) == 0){
            return texture_cache[i].tex_id;
        }
    }

    GLuint tex = loadTextureFromFile(fullpath);
    if (tex == 0)
        return 0;

    strcpy(texture_cache[texture_cache_count].path, fullpath);
    texture_cache[texture_cache_count].tex_id = tex;
    texture_cache_count++;
    
    return tex;
}

GLuint loadTextureFromFile(const char* filename){
    stbi_set_flip_vertically_on_load(true);
    int w, h, channels;
    unsigned char* data = stbi_load(filename, &w, &h, &channels, 4);

    if (!data){
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return tex;
}

void drawModel(GLuint shader, const Model* model){
    glUseProgram(shader);
    for (unsigned int i = 0; i < model->mesh_count; i++){
        const Mesh* m = &model->meshes[i];

        if (m->texture_id != 0){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m->texture_id);
        }

        setUniformInt(shader, "material.diffuse", 0);
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