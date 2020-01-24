#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;	
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 colour;
layout (location = 4) in vec3 biTangent;
layout (location = 5) in vec3 tangent;

out mat3 tangentSpace0;
out vec2 texCoord0;
out vec3 normal0; 
out vec3 colour0;

out vec3 FragPos;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main()
{
	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	vec4 viewPos = viewMatrix * vec4(worldPos.xyz, 1.0);
	vec4 clipPos = projectionMatrix * viewPos;
	gl_Position = clipPos;

	FragPos = worldPos.xyz;

	mat3 normalMat = transpose(inverse(mat3(modelMatrix)));

	texCoord0 = textureCoord;
	colour0 = colour;
	//normal0 = (modelMatrix * vec4(normal, 0.0)).xyz;
	normal0 = normalMat * normal;
	
	vec3 T = normalize(normalMat * tangent);
	vec3 N = normalize(normalMat * normal);
	vec3 B = normalize(normalMat * biTangent);
	T = normalize(T - dot(T, N) * N); //Gram-Schmidt (reorganise vectors to be perpendicular to each other again.)
	
	mat3 TBN = transpose(mat3(
	T.x, B.x, N.x, 
	T.y, B.y, N.y, 
	T.z, B.z, N.z ));

	tangentSpace0 = TBN;
}