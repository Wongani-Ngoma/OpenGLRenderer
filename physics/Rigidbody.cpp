#include "Rigidbody.h"

Rigidbody::Rigidbody(glm::vec3 position, glm::vec3 size, glm::vec3 rotation, float mass)
    : position(position), size(size), rotation(rotation), mass(mass)
{

}

void Rigidbody::update(float dt) {
    velocity += acceleration * dt;
    position += velocity * dt + 0.5f * acceleration * (dt * dt);

}

void Rigidbody::setPosition(glm::vec3 newPos) {
    position = newPos;
}

// applied force will affect acceleration
void Rigidbody::applyForce(glm::vec3 force) {
    acceleration += force / mass;
}

void Rigidbody::applyForce(glm::vec3 direction, float magnitude) {
    applyForce(direction * magnitude);
}

void Rigidbody::applyAcceleration(glm::vec3 a) {
    acceleration += a;
}

void Rigidbody::applyAcceleration(glm::vec3 direction, float magnitude) {
    applyAcceleration(direction * magnitude);
}

void Rigidbody::applyImpulse(glm::vec3 force, float dt){
    velocity += force / mass * dt;
}

void Rigidbody::applyImpulse(glm::vec3 direction, float magnitude, float dt) {
    applyForce(direction * magnitude, dt);
}

void Rigidbody::transferEnergy(float joules) {
    if (joules == 0) {
        return;
    }

    // from : KE = 1/2 * m* v^2
    float deltaV = sqrt(abs(joules) * 2 / mass);

    velocity += joules > 0 ? deltaV : -deltaV;
}
