//
// Created by Philip on 5/12/2022.
//

#ifndef DOGEENGINE_GUI_HPP
#define DOGEENGINE_GUI_HPP

#include "Window.hpp"
#include "Renderer.hpp"
//imgui
#include "imgui/imgui.h"
//imgui backend
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//imgui gizmo
#include "imgui/ImGuizmo.h"
#include <chaiscript/chaiscript.hpp>
#include "PhysicsManager.hpp"
#include "Picker.hpp"
class GUI {
public:
    //renderer for editing values
    Renderer* renderer;
    PhysicsManager* m_physics_manager;

    GUI(Window* window){
        //initialize
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // set style
        ImGui::StyleColorsDark();
        ImGuiStyle * style = &ImGui::GetStyle();
        style->WindowPadding = ImVec2(15, 15);
        style->WindowRounding = 5.0f;
        style->FramePadding = ImVec2(5, 5);
        style->FrameRounding = 4.0f;
        style->ItemSpacing = ImVec2(12, 8);
        style->ItemInnerSpacing = ImVec2(8, 6);
        style->IndentSpacing = 25.0f;
        style->ScrollbarSize = 15.0f;
        style->ScrollbarRounding = 9.0f;
        style->GrabMinSize = 5.0f;
        style->GrabRounding = 3.0f;
        style->ChildBorderSize = 1.0f;

        // set backend
        m_active_window = window->glfw_window;
        ImGui_ImplGlfw_InitForOpenGL(m_active_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

    }
    //render a frame of the gui. Do after 3d render, but before window update.
    void update(){
        //create frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //for gizmo library
        ImGuizmo::BeginFrame();
        //create gui elements
        createElements();
        //update main camera projection
        //TODO make proper resize system in window class. Move somewhere else
        int w, h;
        glfwGetFramebufferSize(m_active_window, &w, &h);
        if(m_view_width != w || m_view_height != h){
            renderer->m_camera->m_aspect = (float)w/h;
            renderer->m_camera->recalculateProjection();
            m_view_width = w;
            m_view_height = h;
        }


        // render frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    ~GUI(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
private:
    int m_view_width = 0;
    int m_view_height = 0;
    //current window for gui
    GLFWwindow* m_active_window;
    //gizmo settings
    ImGuizmo::OPERATION m_current_operation{ImGuizmo::TRANSLATE};
    ImGuizmo::MODE m_gizmo_mode{ImGuizmo::WORLD};
    glm::mat4 m_identity{1};
    //current selected object
    Node* selected = nullptr;
    //traverse the node tree to display it
    void traverseTree(Node* node){
        //create node with unique name
        if(ImGui::TreeNode(node->m_name.c_str())) {
            //allow selection of node
            ImGui::SameLine();
            if(selected != node){

                if(ImGui::Button("select")){
                    if(selected != nullptr){selected->m_selected = false;}
                    selected = node;
                    node->m_selected = true;
                };
            }else{
                ImGui::Text("Selected");
            }
            //add children if open
            for (Node *child: node->m_children) {
                traverseTree(child);
            }
            //go back up
                ImGui::TreePop();
        }
    }
    //show the gizmos
    //uses a camera. Uses a matrix to modify. Returns if gizmo was used.
    bool showGizmo(const Camera* camera, glm::mat4* matrix)
    {
        //show controls for switching gizmo
        if (ImGui::RadioButton("Translate", m_current_operation == ImGuizmo::TRANSLATE)){
            m_current_operation = ImGuizmo::TRANSLATE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", m_current_operation == ImGuizmo::ROTATE)){
            m_current_operation = ImGuizmo::ROTATE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", m_current_operation == ImGuizmo::SCALE)){
            m_current_operation = ImGuizmo::SCALE;
        }
        //show text controls
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents((float*)matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::DragFloat3("Tr", matrixTranslation);
        ImGui::DragFloat3("Rt", matrixRotation);
        ImGui::DragFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)matrix);


        //setup rendering
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        //draw grid and gizmo
        ImGuizmo::DrawGrid((float*)&camera->m_transform, (float*)&camera->m_projection, (float*)&m_identity, 5.f);
        return ImGuizmo::Manipulate((float*)&camera->m_transform, (float*)&camera->m_projection, m_current_operation, m_gizmo_mode, (float*)matrix, NULL);
    }
    //screen for editing a node
    void selectedScreen(Node* node){
        if(node != nullptr){
            ImGui::Text( node->m_name.c_str());
            //set if it is visible
            ImGui::Checkbox("Visible", &node->m_visible);
            ImGui::Checkbox("Outline", &node->m_selected);

                if(ImGui::CollapsingHeader("Physics")) {
                    if(!node->m_physics){
                        static float mass = 0;
                        ImGui::DragFloat("mass", &mass);
                        if(ImGui::Button("Add Physics")) {
                            m_physics_manager->addObject(node, mass);
                        }
                    }else{
                        if(node->m_object != nullptr){
                            ImGui::DragFloat3("Force", (float*)&node->m_object->force,0.1f);
                        }

                    }
                    }



            if(ImGui::CollapsingHeader("Light")){
                static glm::vec3 light_color(1);
                if(node->m_light == nullptr){

                    ImGui::ColorEdit3("color", (float*)&light_color);
                    if(ImGui::Button("Add light")){
                        node->m_light = new Light(light_color, 0);
                        renderer->registerLight(node->m_light);
                    }
                }else{
                    if(  ImGui::ColorEdit3("color", (float*)&node->m_light->m_color)){
                        node->m_light->update();
                    }
                }
            }


            //show gizmo
            if(showGizmo(renderer->m_camera,&node->m_transform)){
                node->updateTransform();
            }
            if(ImGui::Button("deselect")){
                selected->m_selected = false;
                selected = nullptr;
            }

        }else{
            ImGui::Text("None Selected");
        }

    }
    void createElements(){
        static bool isDown = false;
        ImGuiIO& io = ImGui::GetIO();
        if(ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGuizmo::IsOver() && !io.WantCaptureMouse){
            if(!isDown){
                isDown = true;
                ImVec2 m = ImGui::GetMousePos();
                Picker picker(renderer->m_data);
                Node* picked = picker.pick(renderer->m_camera,m.x,m.y, m_view_width,m_view_height,selected);
                if(picked != nullptr){
                    if(selected != nullptr){selected->m_selected = false;}
                    selected = picked;
                    picked->m_selected = true;
                }
            }

        }else{
            isDown = false;
        }

        //create imgui window
        ImGui::Begin("Controls");
        //show fps
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //show background controls
        if(ImGui::ColorEdit3("background", (float*)&renderer->m_background)){
            renderer->updateBackground();
        };
        ImGui::Text("Scene:");
        static char name[10] = {0};
        ImGui::InputText("name" , name,10);
        if(ImGui::Button("Add")) {
            renderer->m_data->addNode(name, glm::mat4(1));
        }
        ImGui::BeginChild("Scrolling", ImVec2(0,0), true);
        //show tree
         for(Node* child : renderer->m_data->m_children){
           traverseTree(child);
        }
         ImGui::EndChild();

        //close window
        ImGui::End();

        ImGui::Begin("Selected:");
        selectedScreen(selected);
        ImGui::End();

        ImGui::Begin("Script:");
        ImGui::InputTextMultiline("script", s, 100);
        if(ImGui::Button("Run")){
            chaiscript::ChaiScript chai;
            chai.add(chaiscript::fun(&GUI::outMessage, this), "out");
            chai.add(chaiscript::fun(&GUI::hide, this), "hide");

            try{
                chai.eval(s);
            }catch (const chaiscript::exception::eval_error &e) {

                 out.push_back("error: " + e.pretty_print());
            }

        }
        if(ImGui::Button("Clear")) {
            out.clear();
        }

        ImGui::BeginChild("Scrolling", ImVec2(0,0), true);
        for(std::string i : out){
            ImGui::Text(i.c_str());
        }

        ImGui::EndChild();
        ImGui::End();

    }
    char s[1000]=  {0};

     void outMessage(const std::string &message) {
        out.push_back(message);
    }
    void hide(const std::string &id) {
        Node* node = renderer->m_data->findNode(id);
        if(node != nullptr){
            node->m_visible = !node->m_visible;
            out.push_back("node hidden");
        }else{
            out.push_back("node not found");
        }
    }

    std::vector<std::string> out;

};

#endif DOGEENGINE_GUI_HPP