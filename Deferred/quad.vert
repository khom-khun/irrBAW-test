#version 430 core
layout(location = 0) in vec4 posAndTex;

layout (location = 0) out vec2 tCoord;

void main()
{
    tCoord = posAndTex.zw;
    gl_Position = vec4(posAndTex.xy, 0.0, 1.0); //only thing preventing the shader from being core-compliant
}
