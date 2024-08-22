#version 330 core

// DEFINING STRUCTS
struct DirectionalLight
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;

	vec3 direction;
};

struct PointLight
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;

	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;

	vec3 position;
	float constant;
	float linear;
	float quadratic;

	vec3 direction;
	float cutOff;
	float outerCutOff;
};

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

// DEFINING INPUT VALUES
// Takes as input fragment position, a texture coordinate, and possibly a normal vector
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;
in vec2 FragPosScreenSpace;

// DEFINING OUTPUT VALUES
out vec4 FragColor;

// DEFINING UNIFORMS
uniform vec3 camPos;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Shadow mapping
uniform sampler2D shadowMap;

// SSAO
uniform sampler2D ssaoMap;
uniform vec2 windowSize;

uniform Material material;

// There is one uniform for each light type, each being an array that contains up to
// 32 of this light type
#define NR_DIR_LIGHTS 32
#define NR_POINT_LIGHTS 32
#define NR_SPOT_LIGHTS 32
uniform DirectionalLight dirLights[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform int nrDirLights;
uniform int nrPointLights;
uniform int nrSpotLights;

// DEFINING FUNCTIONS
// https://learnopengl.com/PBR/Theory
const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

vec3 calcDirLight(DirectionalLight light, vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness)
{
	light.direction = vec3(0.0, -1.0, 0.0);
	vec3 L = -light.direction;
    vec3 H = normalize(V + L);

    vec3 radiance = light.diffuseColor;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.00001) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcPointLight(PointLight light, vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(V + L);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.diffuseColor * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    specular = min(specular, vec3(0.0));

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcSpotLight(SpotLight light, vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness)
{
    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(V + L);

    float theta = dot(L, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.diffuseColor * attenuation * intensity;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.00001) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Get values in range [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of fragment from light's perspective
    float currentDepth = projCoords.z;

    float bias = 0.005;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
	float opacity = material.opacity;
	
    // We start by sampling all the values we need from textures or uniforms
    vec3 albedo;
    if ((material.used_maps & ALBEDO_MAP) != 0)
    {
        vec4 albedoSRGB = texture(material.texture_albedo, TexCoord);
        float r = pow(albedoSRGB.r, 2.2);
        float g = pow(albedoSRGB.g, 2.2);
        float b = pow(albedoSRGB.b, 2.2);
        opacity = pow(albedoSRGB.a, 2.2);
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
    
    float metallic;
    if ((material.used_maps & METALLIC_MAP) != 0)
        metallic = texture(material.texture_metallic, TexCoord).b;
    else
        metallic = material.metallic;
        
    float roughness;
    if ((material.used_maps & ROUGHNESS_MAP) != 0)
        roughness = texture(material.texture_metallic, TexCoord).g;
    else
        roughness = material.roughness;
    
    roughness *= roughness;
    
    float ao;
    if ((material.used_maps & AO_MAP) != 0)
        ao = texture(material.texture_ao, TexCoord).r;
    else
        ao = material.ao;
    
    float shadow = ShadowCalculation(FragPosLightSpace);
    float shadowInverse = 1.0 - shadow;
    
    // Now we can start lighting calculations
    // Normal
    vec3 N = normalize(normalVec);
    // View direction
    vec3 V = normalize(camPos - FragPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // We can start calculating the influence of every light source
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < nrDirLights; i++)
        Lo += calcDirLight(dirLights[i], N, V, F0, albedo, metallic, roughness);

    // Apply shadow mapping on the directional light
    Lo *= shadowInverse;

    for (int i = 0; i < nrPointLights; i++)
        Lo += calcPointLight(pointLights[i], N, V, F0, albedo, metallic, roughness);

    for (int i = 0; i < nrSpotLights; i++)
        Lo += calcSpotLight(spotLights[i], N, V, F0, albedo, metallic, roughness);

    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // Sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    // Sample the SSAO value
    float ssao = texture(ssaoMap, gl_FragCoord.xy / windowSize).r;
    vec3 ambient = (kD * diffuse + specular) * ao;
    // SSAO weighs for 50% of the ambient color to avoid the effect being too strong
    ambient = 0.5 * ambient + 0.5 * ambient * ssao;
	
    vec3 color = ambient + Lo;

    if ((material.used_maps & EMISSIVE_MAP) != 0)
        color += texture(material.texture_emissive, TexCoord).rgb;
        
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
