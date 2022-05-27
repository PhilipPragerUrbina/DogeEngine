#version 330 core
//buffer layout
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coordinate;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

//outputs
out vec2 transfer_texture_coordinates;
out vec3 transfer_normal;
out vec3 transfer_position;
out mat3 transfer_tbn;
out float transfer_t_undefined;
out vec4 transfer_light_space;
//transforms
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    //assign outputs
    transfer_position = vec3(transform * vec4(in_position,1.0));
    transfer_texture_coordinates = in_texture_coordinate;
    transfer_normal = mat3(transpose(inverse(transform))) * in_normal;
    gl_Position = projection * view * transform  * vec4(in_position, 1.0f);
    //calculate TBN matrix
    //check if t is undefined
    if(in_tangent == vec3(0) && in_bitangent == vec3(0)){
        transfer_t_undefined = 1;
    }else{
        transfer_t_undefined = 0;
    }
    transfer_light_space = lightSpaceMatrix * vec4(transfer_position, 1.0);


    vec3 t = normalize(vec3(transform * vec4(in_tangent,   0.0)));
    vec3 b = normalize(vec3(transform * vec4(in_bitangent, 0.0)));
    vec3 n = normalize(vec3(transform * vec4(in_normal,    0.0)));
    transfer_tbn = mat3(t, b, n);
}