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
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

// DEFINING FUNCTION PROTOTYPES
vec3 phong();
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

// DEFINING INPUT VALUES
// Takes as input fragment position, a texture coordinate, and possibly a normal vector
in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

// DEFINING OUTPUT VALUES
out vec4 FragColor;

// DEFINING UNIFORMS
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

uniform bool use_specular_map;
uniform bool use_normal_map;
uniform bool use_height_map;

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

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// Combine results
	vec3 ambient = light.ambientColor;
	vec3 diffuse = light.diffuseColor * (diff * material.diffuse);
	vec3 specular = light.specularColor * (spec * material.specular);

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// Attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Combine results
	vec3 ambient = light.ambientColor;
	vec3 diffuse = light.diffuseColor * (diff * material.diffuse);
	vec3 specular = light.specularColor * (spec * material.specular);
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// Attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// Combine results
	vec3 ambient = light.ambientColor;
	vec3 diffuse = light.diffuseColor * (diff * material.diffuse);
	vec3 specular = light.specularColor * (spec * material.specular);
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0);

	vec3 normalVec;
	if (use_normal_map)
		normalVec = texture(texture_normal1, TexCoord).rgb;
	else
		normalVec = Normal;

	for (int i = 0; i < nrDirLights; i++)
		result += calcDirLight(dirLights[i], normalVec, viewDir);

	for (int i = 0; i < nrPointLights; i++)
		result += calcPointLight(pointLights[i], normalVec, FragPos, viewDir);

	for (int i = 0; i < nrSpotLights; i++)
		result += calcSpotLight(spotLights[i], normalVec, FragPos, viewDir);

	FragColor = vec4(result, 1.0);

	// If texture coordinates are supplied, apply the texture
	if (TexCoord != vec2(0.0, 0.0))
		FragColor *= texture(texture_diffuse1, TexCoord);
}