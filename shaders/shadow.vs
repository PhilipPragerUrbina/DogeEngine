#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coordinate;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

uniform mat4 lightSpaceMatrix;
uniform mat4 transform;

void main()
{
    gl_Position = lightSpaceMatrix * transform * vec4(aPos, 1.0);
}