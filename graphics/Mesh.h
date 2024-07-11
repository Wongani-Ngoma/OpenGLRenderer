#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"
#include "EBO.h"
#include "VAO.h"

typedef struct Vertex Vertex;

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indicies;
    std::vector<Texture> textures;

    VAO vao;
    
    aiColor4D diffuse;
    aiColor4D specular;


    Mesh();
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, std::vector<Texture> textures = {}, bool noTex = true);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, aiColor4D diffuse, aiColor4D spec);

    void render(Shader shader);

    void cleanup();

private:
    bool noTex;
    VBO vbo; //Hold actual vertex data
    EBO ebo; //Contains indicies, each of which references to a group of vertices, eg index 1 might reference to the top left position on the screen (-0.5, 0.5, 0.0)

    void setup();

};
#endif // !MESH_H



