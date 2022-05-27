#version 330 core
//max number of lights that can be assigned in shader
#define MAX_LIGHTS 100

//light struct
struct light{
    vec3 position;
    vec3 color;
};

//color out
out vec4 FragColor;
//inputs
in vec3 transfer_position;
in vec3  transfer_normal;
in vec2 transfer_texture_coordinates;
in mat3 transfer_tbn;
in float transfer_t_undefined;
in vec4 transfer_light_space;
//uniforms
//camera position
uniform vec3 view_position;
//textures
uniform sampler2D color_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_texture;
uniform sampler2D shadow_texture;
//defualt values
uniform vec3 d_color;
uniform vec3 d_specular; //specular is stored in y value,  to gltf
//ambient lighting color
uniform vec3 ambient;
//actual number of lights
uniform int light_number;
//lights array
uniform light lights[MAX_LIGHTS];

uniform bool selected = false;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_texture, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    vec3 light_dir = normalize(lights[0].position - transfer_position);
    float bias = max(0.05 * (1.0 - dot(normalize(transfer_normal), light_dir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    if(projCoords.z > 1.0){
        shadow = 0.0;
    }


    float distance = distance(lights[0].position, transfer_position) * 2;
    return shadow / (1.0f/distance);

}
//point light function
vec3 pointLight(light l, float specular_strength, vec3 normal){
    //get direction and distance
    vec3 light_dir = normalize(l.position - transfer_position);
    float distance = distance(l.position, transfer_position) * 2;
    float difference = max(dot(normal, light_dir), 0.0);

    //get diffuse
    vec3 diffuse = difference * l.color / distance;

    //calculate specular
    const float specular_intensity = 32.0f;
    vec3 view_dir = normalize(view_position-transfer_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir,reflect_dir), 0.0), specular_intensity);
    vec3 specular = specular_strength * spec * l.color * difference ;

    //combine
    vec3 result = (diffuse + specular) ;
    return result;
}

void main()
{

    // obtain normal from normal map
    vec3 new_normal = texture(normal_texture, transfer_texture_coordinates).rgb;
    //if normal map exists
    if(new_normal == vec3(0) || transfer_t_undefined > 0.5){
        //use normal normal
       new_normal = normalize(transfer_normal);
    }else{
        // transform normal
        new_normal = new_normal * 2.0 - 1.0;
        new_normal = normalize(transfer_tbn * new_normal);
    }
    //get object color from texture
    vec3 object_color = texture(color_texture, transfer_texture_coordinates).xyz * d_color;
    //check for transparency
    if(texture(color_texture, transfer_texture_coordinates).w < 0.1){
        discard;
    }
    //highligh selected object
    if(selected){
        vec3 view_dir = normalize(view_position-transfer_position);
        float percentage = dot(view_dir, new_normal);
       object_color *=  percentage + vec3(2,2,0) * (1 - percentage);

    }
    float shadow = ShadowCalculation(transfer_light_space);
    //get specular texture
    float specular_strength =  1.0f - (texture(specular_texture, transfer_texture_coordinates).xyz * d_specular).y;

    //add lights
    vec3 result = ambient + (1.0f-shadow)/3.0f;
    for(int i = 0; i < light_number; i++){
        result += pointLight(lights[i], specular_strength,new_normal);
    }
    //return value
    FragColor = vec4(result * object_color, 1.0);
}//TODO optimize shader(What can be computed once rether than every light)
