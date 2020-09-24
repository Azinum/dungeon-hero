// default.frag

#version 330 core

in vec2 TexCoord;
in vec3 Diffuse;

out vec4 OutColor;

uniform sampler2D Texture;

void main() {
	OutColor = vec4(Diffuse * vec3(texture2D(Texture, TexCoord)), 1);
}
