// default.vert

#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

out vec2 TexCoord;
out vec3 SurfaceNormal;
out vec3 LightDelta;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

vec3 Light = vec3(0, 0.5f, 0);

void main() {
	gl_Position = Projection * View * Model * vec4(Position, 1);
	TexCoord = vec2(TexCoords.x, TexCoords.y);

	SurfaceNormal = (Projection * vec4(Normal, 0.0f)).xyz;
	LightDelta = Light - Position;
}
