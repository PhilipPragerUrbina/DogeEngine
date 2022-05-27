//
// Created by Philip on 5/7/2022.
//

#ifndef DOGEENGINE_MATERIAL_HPP
#define DOGEENGINE_MATERIAL_HPP

#include "Texture.hpp"
#include "Shader.hpp"
//class that contains object materials
class Material {
public:
    //texture data
    Texture* m_color_texture = nullptr;
    Texture* m_specular_texture = nullptr;
    Texture* m_normal_texture = nullptr;
    //default values
    glm::vec3 m_color{1};
    glm::vec3 m_specular{0.5};

    //Set default values on creation
    Material( glm::vec3 color = glm::vec3(1) ,glm::vec3 specular = glm::vec3(0.5)){
        m_color = color;
        m_specular = specular;

    }
    //check if textures exist and then use them
    //also set material values
    void use(Shader* shader){
        shader->setVec3("d_color", m_color);
        if(m_color_texture != nullptr){
            m_color_texture->bind(GL_TEXTURE0);
        }
        shader->setVec3("d_specular", m_color);
        if(m_specular_texture != nullptr){
            m_specular_texture->bind(GL_TEXTURE1);
        }
        if(m_normal_texture != nullptr){
            m_normal_texture->bind(GL_TEXTURE2);
        }

    }

};

#endif DOGEENGINE_MATERIAL_HPP