//
// Created by Philip on 4/30/2022.
//
#ifndef DOGEENGINE_WINDOW_HPP
#define DOGEENGINE_WINDOW_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//glfw window class wrapper
class Window{
public:
    //glfw window pointer
    GLFWwindow* glfw_window = nullptr;
    //create window
    Window(int width, int height, std::string name){
        //set member variables
        m_width = width;
        m_height = height;
        m_name = name;
        //create glfw window
        glfw_window = glfwCreateWindow(m_width, m_height,m_name.c_str(), NULL, NULL);
        if (glfw_window == NULL) {
            std::cerr << "Failed to create window \n";
        }
        std::cout << "created window \n";

    }
    //get the aspect ratio
    float getAspect(){
        glfwGetWindowSize(glfw_window,&m_width,&m_height);
        return (float)m_width/(float)m_height;
    }
    void updateSize(){
        glfwGetWindowSize(glfw_window,&m_width,&m_height);
    }
    //set as active rendering window
    void setMain(){
        glfwMakeContextCurrent(glfw_window);
        //initialize glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD \n";
        }
    }
    //check if open
    bool isOpen(){
        return !glfwWindowShouldClose(glfw_window);
    }
    //update input and update screen
    void update(){
        processInput();
        glfwSwapBuffers(glfw_window);
        //poll events
        glfwPollEvents();
    }
    int m_width;
    int m_height;
private:
    //store attributes

    std::string m_name;


    //close window when needed
    void processInput()
    {
        if(glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(glfw_window, true);
    }
};
#endif DOGEENGINE_WINDOW_HPP
