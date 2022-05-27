//
// Created by Philip on 5/3/2022.
//

#ifndef DOGEENGINE_HPP_LIGHT_HPP
#define DOGEENGINE_HPP_LIGHT_HPP


#include "Node.hpp"


class Light {
public:
    //color and transform
    glm::vec3  m_color;
    glm::mat4 m_transform{1.0f};
    int m_id = 0;
    //set color
    Light(glm::vec3 color , int id){
        m_color = color;
        m_id = id;
    }
    //use light in a shader
    //specify index that light should be assigned to
    void use( Shader* shader){
        last_shader = shader;
        shader->setVec3("lights[" + std::to_string(m_id) + "].position", m_transform[3]);
        shader->setVec3("lights[" + std::to_string(m_id) + "].color", m_color);
    }
    //update position and color for last used shader
    void update(){
        if(last_shader != nullptr){
            last_shader->setVec3("lights[" + std::to_string(m_id) + "].position", m_transform[3]);
            last_shader->setVec3("lights[" + std::to_string(m_id) + "].color", m_color);
        }

    }
    unsigned int depthMap;
    unsigned int depthMapFBO;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    void createShadowMap(){

        glGenFramebuffers(1, &depthMapFBO);



        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        std::cout << "created shadow map \n";
    }
    glm::mat4 lightSpaceMatrix;
    void prepRenderMap(Shader* light_shader){
        light_shader->use();
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightProjection, lightView;

        float near_plane = 1.0f, far_plane = 100.0f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(glm::vec3( m_transform[3].x, m_transform[3].y, m_transform[3].z), glm::vec3( m_transform[3].x-3, m_transform[3].y-3, m_transform[3].z-3), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        light_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    }
    void bind(GLenum tex){
        glActiveTexture(tex);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }

private:
    glm::vec3 m_position;
    Shader* last_shader = nullptr;
};

#endif DOGEENGINE_HPP_LIGHT_HPP
