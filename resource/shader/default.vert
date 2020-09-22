// default.vert

#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

out vec2 TexCoord;
out vec3 LightDelta;
out vec3 SurfaceNormal;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 Light;

void main() {
	LightDelta = Light - Position;
	SurfaceNormal = normalize(mat3(transpose(inverse(Model))) * Normal);

	gl_Position = Projection * View * Model * vec4(Position, 1);
	TexCoord = vec2(TexCoords.x, TexCoords.y);



	/*
	vec3 LightDelta = Light - Position;
	float LightDistance = length(LightDelta);
	vec3 LightNormal = normalize(LightDelta);
	float Attenuation = 1.0 / ((1 + 0.09 * LightDistance) * LightDistance * LightDistance);

	vec3 SurfaceNormal = normalize((Model * vec4(Normal, 0.0)).xyz);
	float DotProduct = max(dot(LightNormal, SurfaceNormal), 0.1f);

	LightFactor = vec3(1, 1, 1) * DotProduct * Attenuation * 2;
	*/
}
