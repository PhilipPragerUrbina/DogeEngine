//
// Created by Philip on 5/1/2022.
//

#ifndef DOGEENGINE_TEXTURE_HPP
#define DOGEENGINE_TEXTURE_HPP

#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

//opengl texture wrapper
class Texture {
public:
    //texture image attributes
    int m_width;
    int m_height;
    int m_channels;

    //opengl texture
    unsigned int texture_id;
    //open from image file
    Texture(std::string path) {
        //correct orientation
        stbi_set_flip_vertically_on_load(true);
        //open image
        unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels,3);
        if (!data) {
            std::cerr << "could not read texture \n";
        }
        //generate texture
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        //set settings
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //add data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //free data
        stbi_image_free(data);
        std::cout << "created texture \n";
        //set back since it sets this program wide
        stbi_set_flip_vertically_on_load(false);
    };
    //create texture from data
    Texture( unsigned char *data, int width, int height, int channels) {
        m_width = width;
        m_height = height;
        m_channels = channels;
        //generate texture
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        //set settings
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //add data depending on channel number
        if(channels == 3){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }else{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        //generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "created texture \n";
    };
    //set texture to an opengl texture slot
    void bind(GLenum tex){
        glActiveTexture(tex);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    ~Texture(){
        glDeleteTextures(1,&texture_id);
    }

};
#endif DOGEENGINE_TEXTURE_HPP
