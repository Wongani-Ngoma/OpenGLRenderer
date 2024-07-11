#include "Model.h"

int Model::id = 0;

Model::Model(bool iDontDoShit) {
}

Model::Model(glm::vec3 pos, glm::vec3 size, glm::vec3 rotation, bool noTex) : noTex(noTex) {
    rigidBody.position = pos;
    rigidBody.rotation = rotation;
    rigidBody.size = size;
    name = "model" + std::to_string(id++);
    ID = id - 1;
}

Model::Model(glm::vec3 pos, glm::vec3 size, bool noTex) {
    rigidBody.position = pos;
    rigidBody.size = size;
    name = "model" + std::to_string(id++);
    ID = id - 1;

}

Model::Model(glm::vec3 pos, bool noTex) {
    rigidBody.position = pos;
    name = "model" + std::to_string(id++);
    ID = id - 1;

}

void Model::init() {}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Could not load model at " << path << std::endl << import.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of("/"));
    processNode(scene->mRootNode, scene);

}

void Model::render(Shader shader, float dt, bool usePhysicsModelMatrix) {

    if (usePhysicsModelMatrix) {
        // the physics model, whose value is set in physics world, already contains position and rotation, but not scale
        physicsModel = glm::scale(physicsModel, rigidBody.size);
        shader.setMat4("model", physicsModel);
    }
    else {
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, rigidBody.position);
        model *= glm::mat4_cast(glm::quat(rigidBody.rotation));
        model = glm::scale(model, rigidBody.size);
        shader.setMat4("model", model);

    }
    shader.setFloat("material.shininess", 0.5f);

    for (Mesh mesh : meshes) {
        mesh.render(shader);
    }
}

void Model::cleanup() {
    for (Mesh mesh : meshes) {
        mesh.cleanup();
    }
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all meshes
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // process all child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Position
        vertex.pos = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // Normals
        vertex.normal = glm::vec3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        );

        // TexCoord
        // assimp mesh can have multiple textures coords

        if (mesh->mTextureCoords[0]) { // if the 0th texture coord exists, we use it
            vertex.texCoord = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        } 
        else {
            vertex.texCoord = glm::vec2(0.0f); // or else we use no texture
        }
        vertices.push_back(vertex);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if (noTex) { // if noTex, use these default materials
            aiColor4D diff(1.0);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);

            aiColor4D spec(1.0);
            aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

            return Mesh(vertices, indices, diff, spec); // this constructor sets noTex to true by default
        }

        // diffuse maps
        std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // specular maps
        std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures, false);
}

std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type) {
    
    std::vector<Texture> textures;

    for (int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::cout << "Loading texture " << str.C_Str() << std::endl;

        // Prevent duplicate loading
        bool skip = false;
        for (int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;

            }

        }

        if (!skip) {
            //not loaded yet
            Texture tex = Texture(directory, str.C_Str(), type);
            tex.load(false);
            textures.push_back(tex);
            textures_loaded.push_back(tex);
        }
    }


    return textures;
}
