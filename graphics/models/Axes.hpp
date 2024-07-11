#include "../Model.h"
#include "../../io/Camera.h"
#include "../../io/Screen.h"

class Axes : public Mesh {
public:
    // we use colors insted of texture coords
    Axes() :
        Mesh
        (
            std::vector<Vertex>
            (
                {
                Vertex(
                    glm::vec3(0.0, 0.0, 0.0),
                    glm::vec3(1.0, 1.0, 1.0),
                    glm::vec2(0.0, 0.0)
                ),
                Vertex(
                    glm::vec3(1.0, 0.0, 0.0),
                    glm::vec3(1.0, 0.0, 0.0),
                    glm::vec2(0.0, 0.0)
                ),
                Vertex(
                    glm::vec3(0.0, 1.0, 0.0),
                    glm::vec3(0.0, 1.0, 0.0),
                    glm::vec2(0.0, 0.0)
                ),
                Vertex(
                    glm::vec3(0.0, 0.0, 1.0),
                    glm::vec3(0.0, 0.0, 1.0),
                    glm::vec2(0.0, 0.0)
                ),
                }
            ),
            std::vector<unsigned int>(
                {
                 0,1,
                 0,2,
                 0,3
                }
            )
               
        )
        
    {
       
    }
    void render(Shader shader, float dt, bool setModel = false) {

        glm::mat4 model(1.0);
        glm::mat4 view(1.0);

        model = glm::translate(model, Camera::defaultCamera.cameraPos + 
            glm::vec3(Camera::defaultCamera.cameraFront) * 0.2f);
        view = Camera::defaultCamera.getViewMatrix();

        model = glm::scale(model, size);

        shader.setMat4("model", model);
        shader.setMat4("view", view);

        // set viewport to the top right corner of the screen and disble depth test
        glViewport(
            Screen::SCR_WIDTH - Screen::SCR_WIDTH / 10, Screen::SCR_HEIGHT - Screen::SCR_HEIGHT / 10,
            Screen::SCR_WIDTH / 10, Screen::SCR_HEIGHT / 10
        );

        vao.Bind();
        glDrawElements(GL_LINES, indicies.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // restore the viewport
        glViewport(0, 0, Screen::SCR_WIDTH, Screen::SCR_HEIGHT);
    }
private:
    glm::vec3 size = glm::vec3(1.0);
};