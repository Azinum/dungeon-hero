// default.vert

#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

out vec2 TexCoord;
// out vec3 SurfaceNormal;
out vec3 Diffuse;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec3 Light;

void main() {
	vec3 LightDelta = Light - Position;
	vec3 SurfaceNormal = normalize(mat3(transpose(inverse(Model))) * Normal);

	float LightDistance = length(LightDelta);
	vec3 LightNormal = normalize(LightDelta);
	float Attenuation = 32.0 / (LightDistance * LightDistance);

	float DotProduct = dot(LightNormal, SurfaceNormal);
	Diffuse = max(DotProduct, 0.1) * vec3(1, 1, 1);

	Diffuse *= Attenuation;

	gl_Position = Projection * View * Model * vec4(Position, 1);
	TexCoord = vec2(TexCoords.x, TexCoords.y);
}
