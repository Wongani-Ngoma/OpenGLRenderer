#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>

class Rigidbody {
public:
    float mass;

    glm::vec3 position;
    glm::vec3 size;
    // the x,y,z of a quat tell the rotation in the corresponding axes, eliminating the need for specifying an axis, when 
    // rotating, just convert the quat to a matrix and multiply
    glm::vec3 rotation; 
    glm::vec3 velocity;
    glm::vec3 acceleration;

    Rigidbody(glm::vec3 position = glm::vec3(0.0), glm::vec3 size = glm::vec3(1.0), glm::vec3 rotation = glm::vec3(0.0), float mass = 0.0);

    void update(float dt);
    void setPosition(glm::vec3 newPos);

    void applyForce(glm::vec3 force); // here force is passed as a vector only
    void applyForce(glm::vec3 direction, float magnitude); // here force is passed as a direction and magnitude

    void applyAcceleration(glm::vec3 a);
    void applyAcceleration(glm::vec3 direction, float magnitude);

    void applyImpulse(glm::vec3 force, float dt);
    void applyImpulse(glm::vec3 direction, float magnitude, float dt);

    void transferEnergy(float joules);
};

#endif // !RIGIDBODY_H
