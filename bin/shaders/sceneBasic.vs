#version 450 core

in vec4 pos;

uniform mat4 vp;

layout(std430, binding = 0) buffer matrixBuffer
{
    mat4 matrices[];
};

void main()
{
    int index = floatBitsToInt(pos.w);
    
    gl_Position = vp * matrices[index] * vec4(pos.xyz, 1);
}