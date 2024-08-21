#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

struct Material
{
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
    float opacity;

	sampler2D texture_albedo;
	sampler2D texture_normal;
	sampler2D texture_metallic;
	sampler2D texture_roughness;
    sampler2D texture_ao;
    sampler2D texture_emissive;

    int used_maps;
};

#define ALBEDO_MAP 1
#define NORMAL_MAP 2
#define METALLIC_MAP 4
#define ROUGHNESS_MAP 8
#define AO_MAP 16
#define OPACITY_MAP 32
#define EMISSIVE_MAP 64

uniform Material material;

void main()
{
    vec3 albedo;
    if ((material.used_maps & ALBEDO_MAP) != 0)
    {
        vec4 albedoSRGB = texture(material.texture_albedo, TexCoord);
        float r = pow(albedoSRGB.r, 2.2);
        float g = pow(albedoSRGB.g, 2.2);
        float b = pow(albedoSRGB.b, 2.2);
        albedo = vec3(r, g, b);
    }
    else
        albedo = material.albedo;

    vec3 normalVec;
    if ((material.used_maps & NORMAL_MAP) != 0)
    {
        normalVec = texture(material.texture_normal, TexCoord).rgb;
        normalVec = normalVec * 2.0 - 1.0;
        normalVec = normalize(TBN * normalVec);
    }
    else
        normalVec = Normal;
    
    // Store fragment position
    gPosition = vec4(FragPos, 1.0);
    // Store normal
    gNormal = vec4(normalVec, 1.0);
    // Store albedo
    gAlbedoSpec = vec4(albedo, 1.0);
}