//
// Created by Philip on 5/5/2022.
//

#ifndef DOGEENGINE_GLTFLOADER_HPP
#define DOGEENGINE_GLTFLOADER_HPP

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <string>
#include <vector>
#include <iostream>
#include "Node.hpp"
#include <glm/gtx/quaternion.hpp>

#include <glm/gtx/string_cast.hpp>


class GLTFLoader {
public:
    //set filename on creation of class
    std::string filename;
    explicit GLTFLoader(const std::string& name) {
        filename = name;
    }
    //load GLTF file
    void loadFile() {
        std::cout << "Loading file: " << filename << "\n";
        //create model with library
        tinygltf::Model model = loadModel();
        //recursively load it
        createModel(model);

    };
    //get scene graph that has been loaded
    SceneGraph* getGraph(){
        return m_contents;
    }

    std::vector<Material*> getMaterials(){
        return m_materials;
    }

private:
    //actual data
    SceneGraph* m_contents = nullptr;
    std::vector<Material *> m_materials;

    //use library to open up model
    tinygltf::Model loadModel() {
        //make sure textures are correct orientation
        stbi_set_flip_vertically_on_load(false);
        tinygltf::TinyGLTF gltf_loader;
        tinygltf::Model model;
        //error handling
        std::string error;
        std::string warning;
        bool result;
        //check if binary gltf file(GLB)
        if (filename.substr(filename.find_last_of(".") + 1) == "glb") {
            result = gltf_loader.LoadBinaryFromFile(&model, &error, &warning, filename);
        }
        else {
            result = gltf_loader.LoadASCIIFromFile(&model, &error, &warning, filename);
        }
        //check for warnings or errors
        if (!warning.empty()) {
            std::cout << "GLTF warning: " << warning << "\n";
        }
        if (!error.empty()) {
            std::cerr << "GLTF error: " << error << "\n";
        }
        if (!result) {
            std::cerr << "GLTF failed";
        }
        else {
            std::cout << "GLTF model loaded successfully \n";
        }
        return model;
    }

    //create meshes
    void createMesh(tinygltf::Model& model, tinygltf::Mesh& mesh,Node* parent_node) {


        //TODO add object names for editor later
        //TODO uniqe identifer and instancing detection
      //  if(m_name_map.count(name)){
       //     GameObject* copy_object = new GameObject(m_name_map.at(name));
      //      std:: cout << "instance generated \n";
      //      return copy_object;
     //   }

        for (size_t i = 0; i < mesh.primitives.size(); ++i) {
            std::vector<float> vertices ;
            std::vector<unsigned int> indices;
            //get indices for correct number of tris
            tinygltf::Accessor index_accessor = model.accessors[mesh.primitives[i].indices];
            tinygltf::BufferView& i_buffer_view = model.bufferViews[index_accessor.bufferView];
            tinygltf::Buffer& i_buffer = model.buffers[i_buffer_view.buffer];
            //2 byte indexes
            const uint16_t* small_indexes;
            //4 byte indexes
            const int* indexes;
            //some have ints of 2, some have of 4
            bool is_16_bit = index_accessor.ByteStride(i_buffer_view) == 2;
            //reinterpret once for efficiency
            if (is_16_bit) {
                small_indexes = reinterpret_cast<const uint16_t*>(&i_buffer.data[i_buffer_view.byteOffset + index_accessor.byteOffset]);

            }else{
                indexes = reinterpret_cast<const int*>(&i_buffer.data[i_buffer_view.byteOffset + index_accessor.byteOffset]);
            }

            //get tri positions
            tinygltf::Accessor& accessor = model.accessors[mesh.primitives[i].attributes["POSITION"]];
            tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
            tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];
            const float* positions = reinterpret_cast<const float*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

            //get tri normals
            tinygltf::Accessor& accessor_1 = model.accessors[mesh.primitives[i].attributes["NORMAL"]];
            tinygltf::BufferView& buffer_view_1 = model.bufferViews[accessor_1.bufferView];
            tinygltf::Buffer& buffer_1 = model.buffers[buffer_view_1.buffer];
            const float* normals = reinterpret_cast<const float*>(&buffer_1.data[buffer_view_1.byteOffset + accessor_1.byteOffset]);

            //get tri texture coordinates
            tinygltf::Accessor& accessor_2 = model.accessors[mesh.primitives[i].attributes["TEXCOORD_0"]];
            tinygltf::BufferView& buffer_view_2 = model.bufferViews[accessor_2.bufferView];
            tinygltf::Buffer& buffer_2 = model.buffers[buffer_view_2.buffer];
            const float* texture_coordinates = reinterpret_cast<const float*>(&buffer_2.data[buffer_view_2.byteOffset + accessor_2.byteOffset]);

            tinygltf::Accessor& accessor_3 = model.accessors[mesh.primitives[i].attributes["TANGENT"]];
            tinygltf::BufferView& buffer_view_3 = model.bufferViews[accessor_3.bufferView];
            tinygltf::Buffer& buffer_3 = model.buffers[buffer_view_3.buffer];
            const float* tangents = reinterpret_cast<const float*>(&buffer_3.data[buffer_view_3.byteOffset + accessor_3.byteOffset]);

            //TODO proper indices handling for less repeats eg: accessor_0.count
            //loop through the indices
            for (size_t i = 0; i < index_accessor.count; i++) {
                //get index
                unsigned int index;
                if (is_16_bit) {
                    index = small_indexes[i];
                }
                else {
                    index = indexes[i];
                }

                indices.push_back(i);

                //set position
                vertices.push_back(positions[index * 3 + 0]);vertices.push_back (positions[index * 3 + 1]);vertices.push_back (positions[index * 3 + 2]);
                //set normal
                vertices.push_back(normals[index * 3 + 0]);vertices.push_back (normals[index * 3 + 1]);vertices.push_back(normals[index * 3 + 2]);
                //set texture coordinates
                vertices.push_back(texture_coordinates[index * 2 + 0]); vertices.push_back(texture_coordinates[index * 2 + 1]);
                //check if it has tangents
                if(accessor_3.bufferView != 0){
                    //set tangent
                    vertices.push_back(tangents[index * 4 + 0]);vertices.push_back (tangents[index * 4 + 1]);vertices.push_back (tangents[index * 4 + 2]);
                    //calculate bitangent
                    glm::vec3 normal(normals[index * 3 + 0],normals[index * 3 + 1],normals[index * 3 + 2]);
                    glm::vec4 tangent(tangents[index * 4 + 0],tangents[index * 4 + 1],tangents[index * 4 + 2],tangents[index * 4 + 3]);
                    glm::vec3 bitangent = glm::cross(normal, glm::vec3(tangent.x,tangent.y,tangent.z)) * tangent.w;
                    //set bitangent
                    vertices.push_back(bitangent.x);vertices.push_back (bitangent.y);vertices.push_back (bitangent.z);
                }else{
                    vertices.push_back(0); vertices.push_back(0); vertices.push_back(0);
                    vertices.push_back(0); vertices.push_back(0); vertices.push_back(0);
                }


            }
            GameObject* new_object = new GameObject();
            new_object->loadGeometry(vertices,indices);
            parent_node->addNode(parent_node->m_name + std::to_string(i),glm::mat4(1.0f), new_object);
            //assign material
            new_object->m_material = m_materials[mesh.primitives[i].material];




        }



    }

    //traverse gltf nodes
    void gltfNode(tinygltf::Model& model,
                  tinygltf::Node& node, Node* parent_node) {
        glm::mat4 transform(1.0f);
        //check if node has matrix
        if (!node.matrix.empty()) {
            transform = glm::make_mat4(node.matrix.data());
            std:: cout << "matrix \n";
        }
        else{
            //does not have matrix. Needs to be created from TRS
            if(!node.translation.empty()){
                glm::vec3 translation = glm::make_vec3(node.translation.data());
                transform = glm::translate(transform, translation);
                //std::cout<< to_string(translation) << " translation \n";
            }
            if(node.rotation.size() > 0){
                glm::quat rotation = glm::make_quat(node.rotation.data());
                   // std::cout<< to_string(rotation) << " rotation \n";
                    glm::mat4 rot_matrix =  glm::toMat4(rotation);
                    transform = transform * rot_matrix;


            }
            if(!node.scale.empty()){
                glm::vec3 scale = glm::make_vec3(node.scale.data());
              //  std::cout<< to_string(scale) << " scale \n";
                transform = glm::scale(transform, scale);
            }

        }
        Camera* camera = nullptr;
        if (node.camera > -1) {
            //has camera
        }
        Node* new_node = parent_node->addNode(node.name ,transform,nullptr,camera);

        //if mesh load vertices
        if (node.mesh > -1) {

            createMesh(model, model.meshes[node.mesh], new_node);
        }

        //then check children
        for (int i : node.children) {
            gltfNode(model, model.nodes[i], new_node);
        }
    }


    //opens gltf model
    void createModel(tinygltf::Model& model) {
        //create scene
        m_contents = new SceneGraph();
        const tinygltf::Scene& scene = model.scenes[model.defaultScene];
        //load materials
        for(auto& gltf_material : model.materials){
            //get default values
            glm::vec3 color = glm::make_vec3(gltf_material.pbrMetallicRoughness.baseColorFactor.data());
            glm::vec3 specular = glm::vec3(gltf_material.pbrMetallicRoughness.roughnessFactor);
            //create material
            Material* new_material = new Material(color, specular);
            //load color texture
            if (gltf_material.pbrMetallicRoughness.baseColorTexture.index != -1){
                auto color_image = model.images[model.textures[gltf_material.pbrMetallicRoughness.baseColorTexture.index].source];
                Texture* color_texture = new Texture(color_image.image.data(), color_image.width, color_image.height, color_image.component);
                new_material->m_color_texture = color_texture;
            }
            //load roughness texture
            if (gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1){
                auto specular_image = model.images[model.textures[gltf_material.pbrMetallicRoughness.metallicRoughnessTexture.index].source];
                Texture* specular_texture = new Texture(specular_image.image.data(), specular_image.width, specular_image.height, specular_image.component);
                new_material->m_specular_texture = specular_texture;
            }
            //load normal texture
            if (gltf_material.normalTexture.index != -1){
                auto normal_image = model.images[model.textures[gltf_material.normalTexture.index].source];
                Texture* normal_texture = new Texture(normal_image.image.data(), normal_image.width, normal_image.height, normal_image.component);
                new_material->m_normal_texture = normal_texture;
            }
            //add material
            m_materials.push_back(new_material);
        }



        //create root node
        Node* model_node = m_contents->addNode(filename, glm::mat4(1.0f));
        //start processing nodes
        std::cout << model.nodes.size() <<  " nodes \n";
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            gltfNode(model, model.nodes[scene.nodes[i]], model_node);
        }
    }

};

#endif DOGEENGINE_GLTFLOADER_HPP
