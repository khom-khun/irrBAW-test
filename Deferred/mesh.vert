#version 330 core
layout(location = 0) in vec3 vPos; 
layout(location = 1) in vec4 vCol;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vNor; 


uniform mat4 MVP;

out vec3 normal;
out vec3 FragPos;
out vec4 Color; //per vertex output color, will be interpolated across the triangle

void main()
{
    gl_Position = MVP*vec4(vPos,1.0); //only thing preventing the shader from being core-compliant
    Color = vCol;
    FragPos = vPos.xyz;
    normal = vNor;
}
