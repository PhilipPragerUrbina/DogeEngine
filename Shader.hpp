//
// Created by Philip on 4/30/2022.
//

#ifndef DOGEENGINE_SHADER_HPP
#define DOGEENGINE_SHADER_HPP
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
//handle opengl shaders
class Shader{
public:
    //set it's source code
    Shader(std::string vertex_path, std::string fragment_path){
        m_fragment_path = fragment_path;
        m_vertex_path = vertex_path;
    }
    //get shader id
    unsigned int getID(){
        return m_id;
    }
    //load the shader and check for errors
    void load() {
        //get source
        std::string vertex_code, fragment_code;
        std::ifstream vertex_file, fragment_file;
        //ensure exceptions
        vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vertex_file.open(m_vertex_path);
            fragment_file.open(m_fragment_path);
            std::stringstream vertex_stream, fragment_stream;
            //read buffer
            vertex_stream << vertex_file.rdbuf();
            fragment_stream << fragment_file.rdbuf();
            //close
            vertex_file.close();
            fragment_file.close();
            //make string
            vertex_code = vertex_stream.str();
            fragment_code = fragment_stream.str();
        }
        catch (std::ifstream::failure e) {
            std::cerr << "Could not read shader \n";
        }
        const char *char_vertex_code = vertex_code.c_str();
        const char *char_fragment_code = fragment_code.c_str();

        //compile
        unsigned int vertex, fragment;
        int success;
        char info_log[512];

        //vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &char_vertex_code, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        //check error
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, info_log);
            std::cerr << "vertex shader error\n" << info_log << "\n";
        };
        //fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &char_fragment_code, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        //check error
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, info_log);
            std::cout << "fragment shader error \n" << info_log << "\n";
        };
        //create program
        m_id = glCreateProgram();
        glAttachShader(m_id, vertex);
        glAttachShader(m_id, fragment);
        glLinkProgram(m_id);
        //check errors
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_id, 512, NULL, info_log);
            std::cout << "linking shader error \n" << info_log << "\n";
        }
        //linked shaders no longer needed
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        std::cout << "created shader \n";
    }
    //set shader as active shader
    void use()
    {
        glUseProgram(m_id);
    }
    //wrappers for opengl uniforms
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


private:
    //shader m_id
    unsigned int m_id;
    std::string m_vertex_path;
    std::string m_fragment_path;
};
#endif DOGEENGINE_SHADER_HPP
