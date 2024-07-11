#include "Mesh.h"

std::vector<struct Vertex> Vertex::genList(float* vertices, int novertices) {
    std::vector<Vertex> ret(novertices);

    int stride = sizeof(Vertex) / sizeof(float);

    for (int i = 0; i < novertices; i++) {
        ret[i].pos = glm::vec3(
            vertices[i * stride + 0],
            vertices[i * stride + 1],
            vertices[i * stride + 2]
        );
        ret[i].normal = glm::vec3(
            vertices[i * stride + 3],
            vertices[i * stride + 4],
            vertices[i * stride + 5]
        );
        ret[i].texCoord = glm::vec2(
            vertices[i * stride + 6],
            vertices[i * stride + 7]
        );
    }

    return ret;
}

Mesh::Mesh() {

}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, std::vector<Texture> textures, bool noTex)
    : vertices(vertices), indicies(indicies), textures(textures), noTex(noTex) {
      setup();

}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicies, aiColor4D diffuse, aiColor4D specular) 
    : vertices(vertices), indicies(indicies), textures(textures), diffuse(diffuse), specular(specular), noTex(true) {
    setup();

}

void Mesh::render(Shader shader) {

    if (noTex) {
        //materials
        shader.set4Float("material.diffuse", diffuse);
        shader.set4Float("material.specular", specular);
        shader.setInt("noTex", 1);
    }
    else {
        //textures
        int diffuseIdx = 0;
        int specularIdx = 0;
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            // retrieve texture info
            std::string name;
            switch (textures[i].type) {
            case aiTextureType_DIFFUSE:
                name = "diffuse" + std::to_string(diffuseIdx++);
                break;
            case aiTextureType_SPECULAR:
                name = "specular" + std::to_string(specularIdx++);
                break;
            }
            //set shader value
            shader.setInt(name, i);
            //bind
            textures[i].bind();
        }
        shader.setInt("noTex", 0);
    
    }

    vao.Bind();
    glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //glActiveTexture(GL_TEXTURE0);
}

void Mesh::cleanup() {
    vao.Delete();
    vbo.Delete();
    ebo.Delete();

}

void Mesh::setup() {

    vao.Bind();

    vbo = VBO(vertices);

    ebo = EBO(indicies);

    //Set the attribute pointers
    //vertex.pos
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    //vertex.Normal
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    //vertex.texCoord
    vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    vao.Unbind();

}

