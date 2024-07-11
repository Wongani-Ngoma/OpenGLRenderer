#pragma once
#ifndef  SCREEN_H
#define SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Abstracts functions related to the screen output, window creation & updating, some callbacks ect
class Screen {
public:
    static unsigned int SCR_WIDTH;
    static unsigned int SCR_HEIGHT;

    static unsigned int SCR_POS_X;
    static unsigned int SCR_POS_Y;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    Screen();

    bool init();

    void setParameters(); //For setting some params like callabacks

    //main loop
    void update(); //called at the begining of each loop, it clears the screen and some bits
    void newFrame(); //called at the end of each loop, swaps buffers and polls events

    //window closing accesor and modifier
    bool shouldClose();
    void setShouldClose(bool shouldClose);

    void toogleFullScreenWindowed();

private:
    GLFWwindow* window;

    bool isFullScreen;
};

#endif // ! SCREEN_H


