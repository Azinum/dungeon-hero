// default.vert

#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

out vec2 TexCoord;
out vec3 SurfaceNormal;
out vec3 FragPos;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
	TexCoord = vec2(TexCoords.x, 1.0 - TexCoords.y);
	SurfaceNormal = mat3(transpose(inverse(Model))) * Normal;
	FragPos = vec3(Model * vec4(Position, 1.0));

	gl_Position = Projection * View * Model * vec4(Position, 1);
}
