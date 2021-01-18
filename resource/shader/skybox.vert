// skybox.vert

#version 130

in vec3 Position;

uniform mat4 Projection;
uniform mat4 View;

out vec3 TexCoord;

void main() {
	vec4 Pos = Projection * View * vec4(Position, 1);
	gl_Position = Pos.xyww;
	TexCoord = Position;
}
