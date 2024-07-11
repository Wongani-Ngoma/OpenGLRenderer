#include "../Model.h"
#include "../../io/Camera.h"
#include "../../io/Keyboard.h"

class Gun : public Model {
public:
    Gun() :
        Model(glm::vec3(0.0), glm::vec3(0.15), false) {
        sholderPos = Camera::defaultCamera.cameraPos +
            glm::vec3(Camera::defaultCamera.cameraFront * 1.5f) +
            glm::vec3(Camera::defaultCamera.cameraUp * -0.22f) +
            glm::vec3(Camera::defaultCamera.cameraRight * 0.34f);

        aimDownSightPos = Camera::defaultCamera.cameraPos +
            glm::vec3(Camera::defaultCamera.cameraFront * 1.5f) +
            glm::vec3(Camera::defaultCamera.cameraUp * -0.3f) +
            glm::vec3(Camera::defaultCamera.cameraRight * 0.0f);

        currentPos = sholderPos;
        hasAimedDownSight = false;
    }

    void render(Shader shader, float dt, bool setModel = false) {
        glm::mat4 model = glm::mat4(1.0);

        // TRANSLATION
        rigidBody.position = currentPos;
        model = glm::translate(model, rigidBody.position);

        // ROTATION
        // rotation due to pitch - rotate around cameraRight using dot product
        float theta = acos(glm::dot(Camera::defaultCamera.worldUp, Camera::defaultCamera.cameraFront) /
            glm::length(Camera::defaultCamera.cameraUp) / glm::length(Camera::defaultCamera.cameraFront));
        model = glm::rotate(model, atanf(1) * 2 - theta, Camera::defaultCamera.cameraRight); // offset by pi/2 radians because angle between cameraFront and gunFront

        // rotation due to yaw - rotate around cameraUp using dot product
        glm::vec2 front2D = glm::vec2(Camera::defaultCamera.cameraFront.x, Camera::defaultCamera.cameraFront.z);
        theta = acos(glm::dot(glm::vec2(1.0, 0.0), front2D) / glm::length(front2D));
        model = glm::rotate(model, (Camera::defaultCamera.cameraFront.z < 0) ? theta : -theta, Camera::defaultCamera.worldUp);

        
        // SCALE
        model = glm::scale(model, rigidBody.size);
        shader.setMat4("model", model);

        Model::render(shader, dt, false);

    }

    void aimDownSight() {
        currentPos = aimDownSightPos;
        hasAimedDownSight = true;
    }

    void putOnSholder() {
        currentPos = sholderPos;
        hasAimedDownSight = false;
    }

    // updates the position of ADS and shoulder sight based on the current camera's position
    void updatePositions() {
        sholderPos = Camera::defaultCamera.cameraPos +
            glm::vec3(Camera::defaultCamera.cameraFront * 1.5f) +
            glm::vec3(Camera::defaultCamera.cameraUp * -0.32f) +
            glm::vec3(Camera::defaultCamera.cameraRight * 0.24f);

        aimDownSightPos = Camera::defaultCamera.cameraPos +
            glm::vec3(Camera::defaultCamera.cameraFront * 1.5f) +
            glm::vec3(Camera::defaultCamera.cameraUp * -0.3f) +
            glm::vec3(Camera::defaultCamera.cameraRight * 0.0f);

        currentPos = hasAimedDownSight ? aimDownSightPos : sholderPos;
    }

    void processInput() {
        if (Keyboard::keyWentDown(GLFW_KEY_TAB)) {
            if (hasAimedDownSight) {
                currentPos = sholderPos;
                hasAimedDownSight = false;
            }
            else {
                currentPos = aimDownSightPos;
                hasAimedDownSight = true;
            }
        }
        
        /*if (Keyboard::key(GLFW_KEY_D)) {
            xMultiplier += 0.01;
            std::cout << "xMultiplier: " << xMultiplier << "\n";
        }
        if (Keyboard::key(GLFW_KEY_A)) {
            xMultiplier -= 0.01;
            std::cout << "xMultiplier: " << xMultiplier << "\n";
        }
        if (Keyboard::key(GLFW_KEY_W)) {
            yMultiplier += 0.01;
            std::cout << "yMultiplier: " << yMultiplier << "\n";
        }
        if (Keyboard::key(GLFW_KEY_S)) {
            yMultiplier -= 0.01;
            std::cout << "yMultiplier: " << yMultiplier << "\n";
        }*/
    }
    float xMultiplier = 0.3f;
    float yMultiplier = -0.4f;
    bool hasAimedDownSight;
private:
    glm::vec3 sholderPos;
    glm::vec3 aimDownSightPos;
    glm::vec3 currentPos;

};