#version 150

out vec4 out_Color;
in vec3 frag_color;

void main(void)
{
	out_Color = vec4(frag_color, 0.8f);
}
