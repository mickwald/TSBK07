#version 150

out vec4 out_Color;
in vec3 frag_color;
in vec2 texCoord;

void main(void)
{
	/*	
	float a = cos(texCoord.s*10);
	float b = sin(texCoord.s*10);
	out_Color = vec4(a, b, 1.0, 0.0);
	*/
	out_Color = vec4(max(0, cos(texCoord.s*5)), max(0, sin(texCoord.t*10)), 0.5, 0.8f);
	
}
