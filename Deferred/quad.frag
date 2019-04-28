#version 430 core

layout(location = 0) out vec4 outcolor;

layout (location = 0) in vec2 tCoord;
layout(binding = 0) uniform sampler2D albedo;
layout(binding = 1) uniform sampler2D normal;
layout(binding = 2) uniform sampler2D depth;

struct lightData_t{
    vec4 pos;
};

layout(std140, binding = 0) uniform Lights{
    lightData_t light;
};

uniform mat4 inPV;

void main()
{

    float d = texture(depth, tCoord).r;

    vec4 clipLoc;
    clipLoc.xy = tCoord * 2.0 - 1.0;
    clipLoc.z = d * 2.0 - 1.0;
    clipLoc.w = 1.0;
    vec4 location = inPV * clipLoc;
    
    vec3 norm = normalize(texture(normal, tCoord).xyz);
    vec3 lightDir = normalize(light.pos - location).xyz;

    vec3 color = texture(albedo, tCoord).xyz;
    

    float ambient = 0.2;
    float diffuse = 0.8;
    float cos_theta_term = max(dot(norm, lightDir), 0.0);

    outcolor = vec4((ambient + diffuse * cos_theta_term) * color, 1.0);
}
