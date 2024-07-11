#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Mesh.h"
#include "../physics/Rigidbody.h"

class Model {
public:
    Model(bool iDontDoShit);
    Model(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0), bool noTex = false);
    Model(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f),bool noTex = false);
    Model(glm::vec3 pos = glm::vec3(0.0f), bool noTex = false);

    void init(); //Overriden by subclasses
    void loadModel(std::string path);
    void render(Shader shader, float dt, bool usePhysicsModel = true);
    void cleanup();

    Rigidbody rigidBody; // when a rigid body is created with no args, it has all values set to default
    glm::highp_quat rot;
    std::string name;
    int ID;
    glm::mat4 physicsModel;

protected:
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::vector<Texture> textures;

    std::string directory;
    bool noTex;

    static int id;
};
#endif // !MODEL_H



