#version 150

out vec4 out_Color;
in vec3 frag_color;
in vec2 texCoord;
uniform sampler2D texUnit;
in vec3 fragNormal;

void main(void)
{
	const vec3 light = vec3 (0.58,0.58,0.58);
	const float kd=0.95;
	float shade;	
	shade = kd * dot(normalize(fragNormal), light);
	shade = clamp(shade, 0, 1);
	out_Color = texture(texUnit, texCoord) * shade;
}
