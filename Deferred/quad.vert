#version 430 core
layout(location = 0) in vec2 vPos; //only a 3d position is passed from irrlicht, but last (the W) coordinate gets filled with default 1.0
layout(location = 1) in vec2 vTex; //only a 3d position is passed from irrlicht, but last (the W) coordinate gets filled with default 1.0


layout (location = 0) out vec2 tCoord;

void main()
{
    tCoord = vTex;
    gl_Position = vec4(vPos, 0.0, 1.0); //only thing preventing the shader from being core-compliant
}
