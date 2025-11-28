#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../include/mesh.h"
#include "../include/shader.h"
#include "../include/cglm/cglm.h"
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/stb/stb_image.h"
#include "../include/assimp/cimport.h" // For the C API functions
#include "../include/assimp/scene.h"   // For aiScene, aiMesh, etc.
#include "../include/assimp/postprocess.h" // For aiProcess_... flags




void Mesh(struct Mesh mesh){
    GLuint EBO, VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, mesh.numOfVertices * sizeof(struct Vertex), mesh.vertices, GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.numOfIndices * sizeof(unsigned int), mesh.indices, GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, TexCoord));

    glBindVertexArray(0);
}

void Draw(GLuint shader, struct Mesh mesh){
    GLuint EBO, VAO, VBO;
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < mesh.numOfTextures; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        char number[2];
        char* name = mesh.textures[i].type;
        if(strcmp(name, "texture_diffuse") == 0)
            sprintf(number, "%d", diffuseNr++);
        else if(strcmp(name, "texture_specular") == 0)
            sprintf(number, "%d", specularNr++);
        char str[20] = "material.";
        sprintf(str + strlen(str), "%s", name);
        sprintf(str + strlen(str), "%s", number);
        
        setUniformInt(shader, str, i);
        glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, mesh.numOfIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


struct Model createModel(char* path){
    struct Model model;
    loadModel(path, model);
    return model;
}



void drawModel(GLuint shader, struct Model model){
    for(unsigned int i = 0; i < model.numOfMeshes; i++){
        Draw(shader, model.meshes[i]);
    }
}

void loadModel(char* path, struct Model model){
    const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        printf("error assimp: %s\n", aiGetErrorString());
        return;
    }
    processNode(scene->mRootNode, scene, path, model);
}

void processNode(struct aiNode *node, const struct aiScene *scene, char* path, struct Model model)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        processMesh(mesh, scene, path, model);			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, path, model);
    }
}


struct Mesh processMesh(struct aiMesh *mesh, const struct aiScene *scene, char* path, struct Model model)
{
    struct Mesh myMesh;
    myMesh.numOfVertices = mesh->mNumVertices;
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // process vertex positions, normals and texture coordinates
        struct Vertex vertex;
        vertex.Position[0] = mesh->mVertices[i].x;
        vertex.Position[1] = mesh->mVertices[i].y;
        vertex.Position[2] = mesh->mVertices[i].z;
        vertex.Normal[0] = mesh->mNormals[i].x;
        vertex.Normal[1] = mesh->mNormals[i].y;
        vertex.Normal[2] = mesh->mNormals[i].z;
        if (mesh->mTextureCoords[0]){
            vertex.TexCoord[0] = mesh->mTextureCoords[0][i].x;
            vertex.TexCoord[1] = mesh->mTextureCoords[0][i].y;
        }else{
            vertex.TexCoord[0] = 0.0f;
            vertex.TexCoord[1] = 0.0f;
        }

        myMesh.vertices[i] = vertex;
    }
    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++){
                myMesh.indices[myMesh.numOfIndices] = face.mIndices[j];
            myMesh.numOfIndices += 1;
        }
    }  
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        struct Texture* diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", path, model);
        for (unsigned int i = 0; i < aiGetMaterialTextureCount(material, aiTextureType_DIFFUSE); i++){
            myMesh.textures[myMesh.numOfTextures] = diffuseTextures[i];
            myMesh.numOfTextures += 1;
        }
        struct Texture* specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", path, model);
        for (unsigned int i = 0; i < aiGetMaterialTextureCount(material, aiTextureType_SPECULAR); i++){
            myMesh.textures[myMesh.numOfTextures] = specularTextures[i];
            myMesh.numOfTextures += 1;
        }
    }

    model.meshes[model.numOfMeshes] = myMesh;
    model.numOfMeshes += 1;
    return myMesh;
}  

struct Texture* loadMaterialTextures(struct aiMaterial *mat, enum aiTextureType type, char* typeName, char* path, struct Model model){
    struct Texture* textures;
    for(unsigned int i = 0; i < aiGetMaterialTextureCount(mat, type); i++){
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL); //null for now
        bool skip = false;
        for (unsigned int j = 0; j < model.numOfTextures; j++){
            if (strcmp(model.loadedTextures[j].path, str.data) == 0){
                skip = true;
                break;
            }
        }
        if (!skip){
            struct Texture texture;
            char *pos = strrchr(path, '/');
            *pos = '\0';
            texture.id = TextureFromFile(str.data, path);
            texture.type = typeName;
            texture.path = str.data;
            textures[i] = texture;
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, char* directory)
{
    sprintf(directory, "%c", '/');
    sprintf(directory, "%s", path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(directory, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        printf("Texture failed to load at path: %s\n", directory);
        stbi_image_free(data);
    }

    return textureID;
}
