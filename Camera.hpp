//
// Created by Philip on 5/3/2022.
//

#ifndef DOGEENGINE_CAMERA_HPP
#define DOGEENGINE_CAMERA_HPP
#include "GameObject.hpp"

class Camera {
public:
    //settings
    float m_aspect, m_near, m_far, m_fov;

    //create camera projections. Fov is in degrees.
    Camera(float aspect = 1, float fov = 45, float near = 0.01, float far = 1000.0f){
        //set values for later
       m_aspect = aspect;
       m_near = near;
       m_far = far;
       m_fov = fov;
        //calculate projection
      recalculateProjection();
      std::cout << "created camera \n";
    }

    //recalculate projection matrix if member settings have changed
    void recalculateProjection(){
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    }

    //update camera transform from position, direction, and up
    void updateTransform(){
        //update transform
        m_transform = glm::lookAt(m_position, m_position + m_direction, m_up);
    }

    //use/update camera for shader. Shader must have projection and view uniforms
    void update(Shader* shader){
        // use shader
        shader->use();
        //set uniforms
        glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "view"), 1, GL_FALSE, &m_transform[0][0]);
        glUniformMatrix4fv( glGetUniformLocation(shader->getID(), "projection"), 1, GL_FALSE, &m_projection[0][0]);
        //update shader camera
        shader->setVec3("view_position",glm::vec3(m_position));

    }
    //attributes
    glm::mat4 m_transform{1.0f};
    glm::vec3 m_position{0.001};
    glm::vec3 m_direction{0,0,1};
    glm::vec3 m_up{0,1,0};
    //camera projection matrix
    glm::mat4 m_projection {1.0f};
private:

};

#endif DOGEENGINE_CAMERA_HPP
