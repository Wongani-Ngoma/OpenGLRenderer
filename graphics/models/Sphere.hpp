#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.h"
#include "ModelArray.hpp"

class Sphere : public Model {
public:
    Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0))
        : Model(pos, size, true) {}

    void init() {
        loadModel("assets/models/sphere.obj");
    }


};

class SphereArray : public ModelArray<Sphere> {
public:
    void init() {
        model = Sphere(glm::vec3(0.0f), glm::vec3(0.25f));
        model.init();
    }
};
#endif // !SPHERE_HPP
