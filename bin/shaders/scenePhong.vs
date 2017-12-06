#version 450 core

in vec4 in_pos;
in vec4 in_normal;
in vec2 in_tcoords;

out vec4 worldPos;
out vec3 normal;
out vec2 tcoords;
out flat int materialIndex;

uniform mat4 vp;

layout(std430, binding = 0) buffer matrixBuffer_model_normal
{
    mat4 matrices[];
};

void main()
{
    int meshIndex = floatBitsToInt(in_pos.w);
    
    materialIndex = floatBitsToInt(in_normal.w);
    gl_Position = vp * matrices[2*meshIndex + 0] * vec4(in_pos.xyz, 1);
    
    normal = (matrices[2*meshIndex + 1] * vec4(in_normal.xyz, 0)).xyz;
    
    tcoords = in_tcoords;
}