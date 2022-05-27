//
// Created by Philip on 5/3/2022.
//

#ifndef DOGEENGINE_HPP_NODE_HPP
#define DOGEENGINE_HPP_NODE_HPP

#include "GameObject.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include "glm/gtx/string_cast.hpp"
//scene graph node
class Node {

public:
    bool m_visible = true;
    bool m_physics = false;
    std::string m_name;
    //local transform(should be edited)
    glm::mat4 m_transform {1.0f};

    //child nodes
    std::vector<Node*> m_children;
    //parent will be null if it is root node
    Node* m_parent = nullptr;

    //can contain one of any entity type TODO add polymorphism?
    GameObject* m_object = nullptr;
    Light* m_light = nullptr;
    Camera* m_camera = nullptr;

    //"constructor" is not really a constructor, since a node should always be created either by another node, or a scene graph object
    Node* addNode( std::string   name,glm::mat4 transform ,GameObject* object = nullptr, Camera* camera = nullptr, Light* light = nullptr){
        static int last_id = 0;

       //create object
        Node* new_node =  new Node();
        //add it's entities
        new_node->m_object = object;
        new_node->m_camera = camera;
        new_node->m_light = light;

        //update its transform
        new_node->m_transform = transform;
        //update global as well, right away
        new_node->updateTransform(m_global_transform);
        //set parent
        new_node->m_parent= this;
        if(name == "" || name == " "){
            name = std::to_string(last_id);
            last_id++;
        }
        new_node->m_name = name;
        //add to children
        m_children.push_back(new_node);
        //return a reference so more nodes can be added toi the new node
        return new_node;

    }
    //update the global transform of a node
    void updateTransform(const glm::mat4& parent_global_transform){
        //update own transform
        m_global_transform = parent_global_transform * m_transform ;
        //update entity transform
        if(m_object != nullptr){
            m_object->m_transform = m_global_transform;
        }
         if(m_light != nullptr){
            m_light->m_transform = m_global_transform;
            m_light->update();
        }
         if(m_camera != nullptr){
            m_camera->m_transform = m_global_transform;
        }
         //update dependencies
        for(Node* child : m_children){
            child->updateTransform(m_global_transform);
        }
    }
    //alternate overload, if just this node and under is being updated and parent has not been moved.
    void updateTransform( ){
        if(m_parent == nullptr){
            updateTransform(glm::mat4(1));
            return;
        }
        updateTransform(m_parent->m_global_transform);
    }
    bool m_selected = false;
    //render a node and it's children
    void render(bool selected = false){

        if(!m_visible){
            return;
        }
        if(m_selected){
            selected = true;
        }
        if(m_object != nullptr){
            //bind rendering info
            m_object->bindShader(selected);
            m_object->bindTexture();
            //bind mesh and get size
            int number =   m_object->bindModel();
            //render
            glDrawElements(GL_TRIANGLES, number, GL_UNSIGNED_INT, 0);

        }
        for(Node* child : m_children){
            child->render(selected);
        }
    }


    void render( Shader* alternative_shader){

        if(!m_visible){
            return;
        }

        if(m_object != nullptr){
            //bind rendering info
            m_object->bindAlternativeShader(alternative_shader);
            m_object->bindTexture();
            //bind mesh and get size
            int number =   m_object->bindModel();
            //render
            glDrawElements(GL_TRIANGLES, number, GL_UNSIGNED_INT, 0);

        }
        for(Node* child : m_children){
            child->render(alternative_shader);
        }
    }


    //apply a shader to node and all it's children. Optionally apply a material as well, if the object does not have one
    void applyShader(Shader* shader, Material*  material = nullptr){
        if(m_object != nullptr){
            m_object->m_shader = shader;
            if(material != nullptr && m_object->m_material == nullptr){
                m_object->m_material = material;
            }
        }
        for(Node* child : m_children){
            child->applyShader(shader,material);
        }
    }

    //clean up references
    ~Node(){
        if(m_object != nullptr){
            delete m_object;
        }
        if(m_light != nullptr){
            delete m_light;
        }
        if(m_camera != nullptr){
            delete m_camera;
        }
        for(Node* child : m_children){
            delete child;
        }
    }

//global tranform(computed)
glm::mat4 m_global_transform {1.0f};


    void applyTransform() {
        //update entity transform
        if(m_object != nullptr){
            m_object->m_transform = m_global_transform;
        }
        if(m_light != nullptr){
            m_light->m_transform = m_global_transform;
            m_light->update();
        }
        if(m_camera != nullptr){
            m_camera->m_transform = m_global_transform;
        }
    }
};


//object for passing around the scene graph. Effectively just the top root node
class SceneGraph{
public:

    //children
    std::vector<Node*> m_children;
    //update entire tree transform
    void updateTransform(){
        for(Node* child : m_children){
           child->updateTransform(glm::mat4(1.0f));
        }
    }
    //returns nullptr if none found
    Node* findNode(std::string name){
        for(Node* child : m_children){
            lookForNode(child, name);
        }
       return foundNode;
    }
    //add a node to graph. Almost identical to the node addNode function
    Node* addNode(std::string name, glm::mat4 transform ,GameObject* object = nullptr, Camera* camera = nullptr, Light* light = nullptr){
        Node* new_node =  new Node();
        new_node->m_object = object;
        new_node->m_camera = camera;
        new_node->m_light = light;
        new_node->m_transform = transform;
        new_node->m_name = name;
        new_node->updateTransform(glm::mat4(1.0f));
        m_children.push_back(new_node);
        return new_node;
    }
    //render entire graph
    void render(){
        for(Node* child : m_children){
            child->render();
        }
    }

    void render(Shader* alternative_shader){
        for(Node* child : m_children){
            child->render(alternative_shader);
        }
    }

    //delete tree
    ~SceneGraph(){
        for(Node* child : m_children){
            delete child;
        }
    }
private:
    Node* foundNode = nullptr;
    void lookForNode(Node* node, std::string name){
        if(node->m_name == name){
            foundNode = node;
        }
        if(foundNode != nullptr){
            return;
        }
        for(Node* child : node->m_children){
            lookForNode(child,name);
        }
    }
};

#endif DOGEENGINE_HPP_NODE_HPP
