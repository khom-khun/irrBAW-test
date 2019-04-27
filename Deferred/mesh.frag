#version 330 core

in vec4 Color; //per vertex output color, will be interpolated across the triangle
in vec3 FragPos;
in vec3 normal;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec3 normals;



void main()
{
    albedo = Color;
    normals = normal;
}
