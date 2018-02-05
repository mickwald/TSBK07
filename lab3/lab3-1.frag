#version 150

out vec4 out_Color;
in vec2 texCoord;
uniform sampler2D texUnit;
in vec3 fragNormal;
in vec4 fragColor;
uniform vec4 color;

void main(void)
{
	const vec3 light = vec3 (0.58,0.58,0.58);
	const float kd=0.45f;
	const float ks=0.5f;
	float shade, l_diff, l_amb;	
	float l_spec = 0;
	const float l_l = 1.0f;
	const float l_a = 0.1f;
	l_amb = kd*l_a;
	l_diff = kd * dot(normalize(fragNormal), light);
	shade = clamp(l_diff+l_amb+l_spec, 0, 1);
	out_Color = vec4(0.3f,0.0f,0.0f,1.0f) * shade;
	//out_Color = texture(texUnit,texCoord) * shade;
}
