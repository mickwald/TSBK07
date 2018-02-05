#version 150

out vec4 out_Color;
in vec2 texCoord;
uniform sampler2D texUnit;
in vec3 fragNormal;
in vec4 fragColor;
uniform vec4 color;
uniform int uniWrap;

void main(void)
{
	out_Color = texture(texUnit,texCoord*uniWrap);
}
