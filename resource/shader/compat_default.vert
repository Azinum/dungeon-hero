// compat_default.vert

#version 130

in vec3 Position;
in vec2 TexCoords;
in vec3 Normal;

out vec2 TexCoord;
out vec3 SurfaceNormal;
out vec3 FragPos;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
	TexCoord = vec2(TexCoords.x, 1.0 - TexCoords.y);
	SurfaceNormal = Normal;
	FragPos = vec3(Model * vec4(Position, 1.0));

	gl_Position = Projection * View * Model * vec4(Position, 1);
}
