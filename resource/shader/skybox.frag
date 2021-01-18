// skybox.frag

#version 130

in vec3 TexCoord;

out vec4 Color;

uniform samplerCube Texture;

void main() {
	Color = texture(Texture, TexCoord);
}
