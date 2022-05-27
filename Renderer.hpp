//
// Created by Philip on 4/30/2022.
//

#ifndef DOGEENGINE_RENDERER_HPP
#define DOGEENGINE_RENDERER_HPP

#include "Node.hpp"
#include "Helper.hpp"

//class for rendering to opengl
class Renderer{
public:
    glm::vec3 m_background{0.1f};
    //camera for scene
    Camera* m_camera;
    //data to render
    SceneGraph* m_data;
    Shader* shadow_shader;
    //initialize renderer
    Renderer(){

        //load shaodow shader
        shadow_shader = new Shader(Helper::relativePath("shadow.vs"),Helper::relativePath("shadow.fs"));
        shadow_shader->load();
        //compile and load main shader
        main_shader = new Shader(Helper::relativePath("main.vs"),Helper::relativePath("main.fs"));
        main_shader->load();
        main_shader->use();
        //set texture positions
        main_shader->setInt("color_texture", 0);
        main_shader->setInt("specular_texture", 1);
        main_shader->setInt("normal_texture", 2);
        main_shader->setInt("shadow_texture", 3);

         //add default lights
        addLight(glm::vec3(3, 3, 3), glm::vec3(1.0f));
       // addLight(glm::vec3(0,0.5,0), glm::vec3(0.4f));
       //set default background
        main_shader->setVec3("ambient", m_background);
        lights[0]->createShadowMap();

        //create test texture
        Texture* test_texture = new Texture(Helper::relativePath("test.jpg"));
        test_material = new Material();
        test_material->m_color_texture = test_texture;
        //enable options
        glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    }
    void updateBackground(){
        main_shader->setVec3("ambient", m_background);
    }
    void addLight(glm::vec3 position, glm::vec3 color){
        Light* new_light =  new Light(color,lights.size());
        new_light->m_transform = glm::translate(new_light->m_transform, position);
        new_light->use( main_shader);
        lights.push_back(new_light);
        main_shader->setInt("light_number",lights.size());
    }
    //set light id and register it.
    void registerLight(Light* light){


            light->m_id = lights.size();
            lights.push_back(light);
            main_shader->setInt("light_number",lights.size());

        light->use( main_shader);

    }
   //init the scene camera
    void createCamera(float aspect){
        m_camera = new Camera(aspect);
        m_camera->update(main_shader);

    }

    void loadGraph(SceneGraph* data){
        m_data = data;
        m_data->m_children[0]->applyShader(main_shader,test_material);
        m_data->updateTransform();
    }

    //render a frame
    void render(int w, int h){
        lights[0]->m_transform[3].x = m_camera->m_position.x ;
        lights[0]->m_transform[3].y = m_camera->m_position.y + 3;
        lights[0]->m_transform[3].z = m_camera->m_position.z ;
        lights[0]->use(main_shader);
        lights[0]->prepRenderMap(shadow_shader);
        glCullFace(GL_FRONT);
        m_data->render(shadow_shader);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, w, h);
        main_shader->use();
        lights[0]->bind(GL_TEXTURE3);
        main_shader->setMat4("lightSpaceMatrix", lights[0]->lightSpaceMatrix);
        m_camera->update(main_shader);
        //background color
        glClearColor(m_background.x,m_background.y,m_background.z,1.0f);
        //clearing buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //render data
        m_data->render();
    }
    //delete stuff here
    ~Renderer(){
delete main_shader;
delete test_material;
    }


private:
    //main shader for rendering
    Shader* main_shader;
    //test grid texture
    Material* test_material;
    //lights: TODO move to scene graph
    std::vector<Light*> lights;
};
#endif DOGEENGINE_RENDERER_HPP
