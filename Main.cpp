#define NUM_OF_CUBES 5

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Shader.h"
#include "graphics/Texture.h"

#include "graphics/Light.h"
#include "graphics/Model.h"

#include "graphics/models/Cube.hpp"
#include "graphics/models/Lamp.hpp"
#include "graphics/models/Gun.hpp"
#include "graphics/models/Axes.hpp"
#include "graphics/models/Sphere.hpp";

#include "physics/PhysicsWorld.h"

#include "io/Keyboard.h"
#include "io/Joystick.h"
#include "io/Mouse.h"
#include "io/Camera.h"
#include "io/Screen.h"


Screen screen;
Joystick joystick(0);
PhysicsWorld physicsWorld;
Camera Camera::defaultCamera(glm::vec3(24.0f, 14.0f, 21.0f));
const int numOfBoxes = 4;
Model ground(true);
Model boxes[numOfBoxes] = { Model(true), Model(true), Model(true), Model(true) };
//SphereArray launchObjects;
//Gun gun;

float deltaTime;
float lastFrame = 0.0f;

bool flashlightOn = true;

int err;

static void launchItem(float dt);
void processInput(double dt);

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //for debugging
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__ //For macOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPACT, GL_TRUE);
#endif

    if (!screen.init()) {
        std::cout << "Could not create window\n";
        glfwTerminate();
        return -1;

    }

    //Glad needs to be initilized before any OpenGL calls
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initilize glad\n";
        glfwTerminate();
		return -1;
    }

    screen.setParameters();

    physicsWorld.init();

    //Shaders
    Shader shader = Shader("assets/shaders/Object.vert", "assets/shaders/Object.frag");
    Shader outliningShader = Shader("assets/shaders/outlining.vert", "assets/shaders/outlining.frag");
    Shader axesShader = Shader("assets/shaders/AxesShader.vert", "assets/shaders/AxesShader.frag");
    

    //Model person(glm::vec3(0.0), glm::vec3(1.0), true);
    //person.loadModel("assets/models/person.obj");

    ground = Model(glm::vec3(0.0, 0.0, 0.0), glm::vec3(10.0, 1.0, 10.0), true);
    ground.rigidBody.mass = 0.0;
    ground.loadModel("assets/models/cube.obj");
    for (int i = 0; i < numOfBoxes; i++) {
        boxes[i] = Model(glm::vec3(0.0, 10 + (i * 2), 0.0), true);
        boxes[i].rigidBody.mass = 1.0;
        boxes[i].loadModel("assets/models/cube.obj");
        //boxes[i].rigidBody.size = glm::vec3()
        physicsWorld.addRigidBodyFromModel(&boxes[i]);
    }

    physicsWorld.addRigidBodyFromModel(&ground);

    //gun.loadModel("assets/models/m4a2-cs2/source/m4a2.obj");

    Axes ao;

    DirLight dirLight = { glm::vec3(-0.2, -1.0, -0.3),
        glm::vec4(0.1, 0.1, 0.1, 1.0),
        glm::vec4(0.4, 0.4, 0.4, 1.0f), 
        glm::vec4(0.75, 0.75, 0.75, 1.0f) 
    };

    glm::vec3 pointLightPositions[] = {
            glm::vec3(2.3f, 5.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, 12.0f),
    };

    glm::vec4 ambient(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 diffuse(0.8f, 0.8f, 0.8f, 1.0f);
    glm::vec4 specular(0.5f, 0.5f, 0.5f, 1.0f);

    float k0 = 1.0f;
    float k1 = 0.08f;
    float k2 = 0.032f;

    PointLight pointLight = {
        glm::vec3(10, 10, 10),
        k0, k1, k2,
        ambient,
        diffuse,
        specular
    };

    LampArray lampArray;
    lampArray.init();

    for (unsigned int i = 0; i < 2; i++) {
        lampArray.pointLights.push_back( { pointLightPositions[i], k0,k1,k2, ambient, diffuse, specular } );
    }

    SpotLight spotLight = {
        Camera::defaultCamera.cameraPos, Camera::defaultCamera.cameraFront,
       1.0f, 0.07f, 0.032f,
       glm::cos(glm::radians(20.0)), glm::cos(glm::radians(12.5f)),
       glm::vec4(2.0), glm::vec4(2.0), glm::vec4(2.0)
    };

    joystick.update();
    if (joystick.isPresent()) {
        std::cout << joystick.getName() << " is present.\n";
    }
    else {
        std::cout << "No controller present.\n";
    }


    glViewport(0, 0, screen.SCR_WIDTH, screen.SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    // stencil buffer stuff, for effects like masking and outlining
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    while (!screen.shouldClose() && deltaTime >= 1 / 60) { // cap at 60 hz

        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        //Process input
        processInput(deltaTime); 

        //Render
        screen.update();

        // use the time step that the renderer uses, it may be variable, so dont give a constant or else bullet and the
        // rendere will not be in sync
        physicsWorld.update(deltaTime);

        shader.activate();
        shader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
        shader.setInt("noPointLights", 1);
        //Create transformation for screen
        glm::mat4 view = glm::mat4(1.0f); // from world to camera, specific to each camera
        glm::mat4 projection = glm::mat4(1.0); // general, actually specific to each viewport
        view = Camera::defaultCamera.getViewMatrix();
        projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)screen.SCR_WIDTH / (float)screen.SCR_HEIGHT, Camera::defaultCamera.near, Camera::defaultCamera.far);

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setFloat("near", Camera::defaultCamera.near);
        shader.setFloat("far", Camera::defaultCamera.far);

        //outliningShader.activate();
        //outliningShader.setMat4("view", view);
        //outliningShader.setMat4("projection", projection);
 
        shader.activate();

        dirLight.render(shader);
        pointLight.render(shader, 0);

        ground.render(shader, deltaTime);
        for (int i = 0; i < numOfBoxes; i++) {
            boxes[i].render(shader, deltaTime);
        }

        std::stack<int> removeObjects;
        //for (int i = 0; i < launchObjects.instances.size(); i++) {
            //if (glm::length(Camera::defaultCamera.cameraPos - launchObjects.instances[i].position) > 40) {
            //    removeObjects.push(i);
            //   continue;
            //}
        //}

        for (int i = 0; i < removeObjects.size(); i++) {
            //launchObjects.instances.erase(launchObjects.instances.begin() + removeObjects.top());
            removeObjects.pop();
        }

        //if (launchObjects.instances.size() > 0) {
        //    launchObjects.render(shader, deltaTime);
        //}

        
        axesShader.activate();
        ao.render(axesShader, deltaTime);


        //Send new frame to window
        screen.newFrame();

        err = glGetError();
        if (err) {
            std::cout << err << "\n";
            //break;
        }
    }

    physicsWorld.cleanup();
    //person.cleanup();
    //gun.cleanup();
    //launchObjects.cleanup();
    ao.cleanup();

    lampArray.cleanup();

    glfwTerminate();
    return 0;
}

static void launchItem(float dt) {
    //Rigidbody rb(1.0f, Camera::defaultCamera.cameraPos);
    //rb.applyImpulse(Camera::defaultCamera.cameraFront, 10000.0, dt);
    //launchObjects.instances.push_back(rb);

}

void processInput(double dt) {

    if (Keyboard::key(GLFW_KEY_ESCAPE) || joystick.buttonState(GLFW_JOYSTICK_START)) {
        screen.setShouldClose(true);
    }

    if (Keyboard::key(GLFW_KEY_LEFT_ALT) && Keyboard::keyWentDown(GLFW_KEY_ENTER)) {
        screen.toogleFullScreenWindowed();
    }

    if (Keyboard::keyWentDown(GLFW_KEY_F)) {
        flashlightOn = !flashlightOn;
    }

    if (Keyboard::keyWentDown(GLFW_KEY_A)) {
        physicsWorld.resetSimulation();

        physicsWorld.addRigidBodyFromModel(&ground);
        for (int i = 0; i < numOfBoxes; i++) {
            boxes[i].rigidBody.position.z += 0.2 * i;
            physicsWorld.addRigidBodyFromModel(&boxes[i]);
        }

    }
    Camera::defaultCamera.processInput(joystick, dt);

    //gun.processInput();

}
