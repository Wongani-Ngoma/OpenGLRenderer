#ifndef LAMP_HPP
#define LAMP_HPP

#include "Cube.hpp"
#include "ModelArray.hpp"
#include "../Light.h"

class Lamp : public Cube{
public:
    glm::vec3 lightColor;

    PointLight pointLight;
    Lamp() {}
    Lamp(glm::vec3 lightColor,
        glm::vec4 ambient,
        glm::vec4 diffuse,
        glm::vec4 specular,
        float k0,
        float k1,
        float k2,
        glm::vec3 pos, glm::vec3 size) 
        :lightColor(lightColor),
        pointLight({ pos, k0, k1, k2, ambient, diffuse, specular }),
        Cube(pos, size) {}

    void render(Shader shader, float dt) {
        shader.set3Float("lightColor", lightColor);
        Cube::render(shader, dt);
    }

};

class LampArray : public ModelArray<Lamp> {
public:

    std::vector<PointLight> pointLights;

    void init() {
        model = Lamp(
            glm::vec3(50.0f, 50.0f, 130.0f),
            glm::vec4(5.0, 5.0, 5.0, 5.0),
            glm::vec4(5.0, 5.0, 5.0, 5.0),
            glm::vec4(5.0, 5.0, 5.0, 5.0),
            1.0, 0.07, 0.032,
            glm::vec3(0.0f),
            glm::vec3(1.0)
        );
        model.material = Material::cyan_plastic;
        model.init();
    }

    void render(Shader shader, float dt) {
        for (PointLight& pointLight : pointLights) {
            model.rigidBody.position = pointLight.position;
            shader.set3Float("lightColor", glm::vec3(50.0f, 50.0f, 130.0f));

            model.render(shader, dt);
        }
    }
};
#endif // !LAMP_H
