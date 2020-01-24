#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoord;	
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 colour;
layout (location = 4) in vec3 biTangent;
layout (location = 5) in vec3 tangent;

out vec2 texCoord0;
out vec3 normal0; 
out vec4 colour0;

out vec4 FragPos;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main()
{
	vec4 worldPos = modelMatrix * vec4(position, 1.0);
	vec4 viewPos = viewMatrix * vec4(worldPos.xyz, 1.0);
	vec4 clipPos = projectionMatrix * viewPos;
	gl_Position = clipPos;

	FragPos = worldPos;

	texCoord0 = textureCoord;
	colour0 = vec4(colour, 1.0);
	normal0 = (modelMatrix * vec4(normal, 0.0)).xyz;
}