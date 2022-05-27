//
// Created by Philip on 5/2/2022.
//

#ifndef DOGEENGINE_GAMEOBJECT_HPP
#define DOGEENGINE_GAMEOBJECT_HPP

#include <vector>

#include "Material.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class GameObject {
public:
    //object material
    Material* m_material = nullptr;
    //shader to apply to object
    Shader* m_shader;
    //name of object TODO create uniqe identifier
    std::string m_name;
    //object transform
    glm::mat4 m_transform {1.0f};
    glm::vec3 force{0};

    //init object from texture and shader
    GameObject(Shader* shader, Material* material,std::string name) {
        m_shader = shader;
        m_material = material;
        m_name = name;

    }
    //init object from name
    GameObject(std::string name = "object"){
        m_name = name;
    }
    //create instance from other object(kind of like copy constructor)
    GameObject(GameObject* base){
        m_shader = base->m_shader;
        m_material = base->m_material;
        base->getBuffers(&VAO,&VBO, &EBO);
        m_transform = base->m_transform;
        m_size = base->getSize();
        m_name = base->m_name + "_copy";
    }
    std::vector<float> m_vertices;
    //load model data into buffers
    void loadGeometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices){
        //generate vertex array
        glGenVertexArrays(1, &VAO);
        //generate buffers
        //vertex buffer object
        glGenBuffers(1, &VBO);
        //element buffer object
        glGenBuffers(1, &EBO);
        //bind vertex array
        glBindVertexArray(VAO);
        //copy vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float ), vertices.data(), GL_STATIC_DRAW);
        //copy indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        const int total_size = 14;
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_size * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // vertex normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, total_size * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        //tex coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, total_size * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        //tangents coord attribute
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, total_size * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, total_size * sizeof(float), (void*)(11 * sizeof(float)));
        glEnableVertexAttribArray(4);
        //update size
        m_size = indices.size();
        m_vertices = vertices;
        calculateBoundingBox();
        std::cout << "created model \n";

    }

    //get object buffers(for re-use)
    void getBuffers(unsigned int* out_vao, unsigned int* out_vbo, unsigned int* out_ebo){
        out_vao= &VAO;
        out_ebo = &EBO;
        out_vbo = &VBO;
    }
    //get object vertex number
    int getSize(){
        return  m_size;
    }
    //bind the object's shader for rendering
    void bindShader(bool selected = false){
        m_shader->use();
        m_shader->setBool("selected" , false);
        if(selected){
            m_shader->setBool("selected" , selected);
        }
        //apply transform to shader
        glUniformMatrix4fv(glGetUniformLocation(m_shader->getID(), "transform"), 1, GL_FALSE, glm::value_ptr(m_transform));
    }
    void bindAlternativeShader(Shader* shader){
        shader->use();
        //apply transform to shader
        glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "transform"), 1, GL_FALSE, glm::value_ptr(m_transform));
    }
    //apply texture
    void bindTexture(){
        m_material->use(m_shader);
    }
    //bind vertex array object and return size
    int bindModel(){
        glBindVertexArray(VAO);
        return m_size;
    }
    //delete the object's data
    ~GameObject(){
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

    }
    glm::vec3 m_bounding_min{-1};
    glm::vec3 m_bounding_max{1};
private:
    void calculateBoundingBox(){
        for(int i = 0; i < m_vertices.size(); i+= 14){
            glm::vec3 point(m_vertices[i], m_vertices[i+1],m_vertices[i+2]);
            m_bounding_min = glm::min(m_bounding_min,point);
            m_bounding_max = glm::max(m_bounding_max,point);
        }

    }
    //buffers
    unsigned int VAO,VBO, EBO;
    //vertex count
    int m_size;
};

#endif DOGEENGINE_GAMEOBJECT_HPP
