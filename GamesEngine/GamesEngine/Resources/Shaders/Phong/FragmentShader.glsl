#version 430 core

out vec4 FragColor;	

in vec3 FragPos;
in vec2 texCoord0;
in vec3 normal0;
in vec3 colour0;
in mat3 tangentSpace0;

// Material
struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D bumpMap;
	sampler2D emissionMap;
	vec3 diffuseColour;	
	vec3 specularColour;	
	vec3 emissionColour;
	float shininess;																
};

// Lights
struct DirectionalLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float intensity;

	float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;
uniform vec3 cameraPos;
				
// Declaration
vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float cutoff);

void main()													
{					
	vec3 norm = (255.0/128.0 * texture(material.bumpMap, texCoord0).rgb - 1.0) * (2.0 * normal0 - 1.0); //in tangent space (-1, 1).
	norm = normalize(norm);

	vec3 viewDir = normalize(tangentSpace0 * cameraPos - tangentSpace0 * FragPos);

	vec3 result = CalculateDirLight(directionalLight, norm, viewDir);

	//result += CalculatePointLight(pointLight, norm, FragPos, viewDir, 0.3);	

	//Support for multiple lights. Not implemented yet 
	//for(int i=0; i<NUM_LIGHTS; i++)		//NUM_LIGHTS = number of point lights in the scene. Sent as uniform
	//{
	//	result += CalculatePointLight(pointLight[i], norm, FragPos, viewDir, 0.3);	
	//}
	//
	vec3 emission = vec3(0.0);
	
	emission = texture(material.emissionMap, texCoord0).rgb * material.emissionColour; // * material.emissionAmp;	//emissionAmp is 0 if the texture exists and 1 otherwise
	
	result += emission;

    FragColor = vec4(result , 1.0);
};		


vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(tangentSpace0 * (-light.direction));

	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, texCoord0)) * material.diffuseColour;

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 diffuse = colour0 * light.diffuse * diff * vec3(texture(material.diffuseMap, texCoord0)) * material.diffuseColour;

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specularMap, texCoord0)) * material.specularColour;
	
	return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float cutoff)
{
	vec3 lightDir = normalize(tangentSpace0 * light.position - fragPos);

	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, texCoord0)) * material.diffuseColour;
	
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = colour0 * light.diffuse * diff * vec3(texture(material.diffuseMap, texCoord0)) * material.diffuseColour;
	
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specularMap, texCoord0)) * material.specularColour;

	// Attenuation
	float distance = length(tangentSpace0 * light.position - fragPos);
	float attenuation = light.intensity / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	attenuation = (attenuation - cutoff) / (1 - cutoff);
	attenuation = max(attenuation, 0.0);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}