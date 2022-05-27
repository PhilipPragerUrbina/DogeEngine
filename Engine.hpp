//
// Created by Philip on 4/30/2022.
//
#ifndef DOGEENGINE_ENGINE_HPP
#define DOGEENGINE_ENGINE_HPP
#include "GUI.hpp"
#include "GLTFLoader.hpp"
#include "PhysicsManager.hpp"

//main app class
class Engine{
public:
    Engine(std::string name, int width, int height){
        //set engine name
        m_name = name;
        //initialize glfw
        glfwInit();
        //set versions
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        std::cout << "engine started \n";

        //create window
        m_window = new Window(width, height, m_name);
        m_window->setMain();
        //create renderer
        m_renderer = new Renderer();
        //create gui
        m_gui = new GUI(m_window);
        //set gui renderer so it can update it's values
        m_gui->renderer = m_renderer;
        //create default camera TODO move to scene tree, add automatic aspect getting and updating
        m_renderer->createCamera(m_window->getAspect());
        //set main camera for movement  TODO move to scripting
        main_camera = m_renderer->m_camera;

       m_physics_manager = new PhysicsManager();
       m_gui->m_physics_manager = m_physics_manager;

    }
    void loadFile(std::string filename){
        GLTFLoader loader(filename);
        loader.loadFile();
        //TODO allow combination of multiple graphs.
        m_scene = loader.getGraph();
        //TODO Allow easy changing of renderer graphs(have default material from start)
        m_renderer->loadGraph(m_scene);
    }

    //run main loop
    void Run() {
        while(m_window->isOpen())
        {
            m_physics_manager->update();
            //render
            m_window->updateSize();
            m_renderer->render(m_window->m_width, m_window->m_height);
            //render gui
            m_gui->update();
            //update window buffers
            m_window->update();
            processInput(m_window->glfw_window, m_renderer);

        }
    }
    //handle input
    bool rel1 = true;
    bool rel2 = true; bool firstMouse = true;
    float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch =  0.0f;
    float lastX =  800.0f / 2.0;
    float lastY =  600.0 / 2.0;
    bool isOut = true ;
    void processInput(GLFWwindow *window, Renderer* renderer)
    {
         float camera_speed = 0.05f; // adjust accordingly
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera_speed = 0.5;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            main_camera->m_position += camera_speed * main_camera->m_direction ;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            main_camera->m_position -= camera_speed * main_camera->m_direction ;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            main_camera->m_position  -= glm::normalize(glm::cross( main_camera->m_direction, main_camera->m_up)) * camera_speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            main_camera->m_position  += glm::normalize(glm::cross( main_camera->m_direction, main_camera->m_up)) * camera_speed;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
            rel1 = true;

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
            rel2 = true;

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && rel1){
            rel1 = false;
            renderer->addLight(main_camera->m_position, glm::vec3(Helper::randomFloat(), Helper::randomFloat(), Helper::randomFloat()));
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && rel2){
            rel2 = false;
            renderer->addLight(main_camera->m_position, glm::vec3(0.5));
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            glfwSetInputMode(m_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            isOut = false;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            glfwSetInputMode(m_window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
            isOut = true;
        }
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
      if(isOut){
          return;
      }

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        main_camera->m_direction = glm::normalize(direction);
        main_camera->updateTransform();
    }

    //clean up
    ~Engine(){
        //destroy objects
        delete m_window;
        delete m_renderer;
        delete m_gui;
        delete m_scene;
        //close glfw
        glfwTerminate();
    }

private:
    //engine name
    std::string m_name;
    //main engine window
    Window* m_window;
    //engine renderer
    Renderer* m_renderer;
    //engine ui
    GUI* m_gui;
    //main engine camera for movement
    Camera* main_camera;

    //engine scene
    SceneGraph* m_scene;
    PhysicsManager* m_physics_manager;

};
#endif DOGEENGINE_ENGINE_HPP





