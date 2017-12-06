#version 450 core

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

in vec4 worldPos;
in vec3 normal;
in vec2 tcoords;
in flat int materialIndex;

out vec4 outColor;

uniform vec3 lightPos;
uniform vec3 cameraPos;

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
    
    //If negative - material has no texture
    int textureIndex;

    //Alignment to 128bit, in terms of size
    float aligmetVariableDoNotUse1, aligmetVariableDoNotUse2;
};

layout(std430, binding = 1) buffer textureBuffer
{
    uint64_t textures[];
};

layout(std430, binding = 2) buffer materialBuffer
{
    Material materials[];
};

void main()
{
    vec3 N = normalize(normal);
    if(!gl_FrontFacing)
        N *= -1;
    
    vec3 wPos = worldPos.xyz / worldPos.w;
    
    Material mat = materials[materialIndex];

    vec3 color = mat.ambient.xyz;
    
    vec3 L = normalize(lightPos - wPos);
    
    float diffuseFactor = dot(N, L);
    
    if(diffuseFactor>0)
    {
        vec4 texColor = vec4(1, 1, 1, 1);
        
        if(mat.textureIndex>=0)
            texColor = texture(sampler2D(textures[mat.textureIndex]), tcoords);
            
        color += diffuseFactor * mat.diffuse.xyz * texColor.xyz;
        
        if(mat.shininess>0)
        {
            vec3 R = normalize(reflect(-L, N));
            vec3 V = normalize(cameraPos - wPos);
        
            float specularFactor = dot(R, V);
        
            if(specularFactor>0)
                color += pow(specularFactor, mat.shininess) * mat.specular.xyz;
        }
    }
    
    outColor = vec4(color, 1);
}