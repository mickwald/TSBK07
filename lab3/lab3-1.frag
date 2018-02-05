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
	const float kd=0.95;
	float shade;	
	shade = kd * dot(normalize(fragNormal), light);
	shade = clamp(shade, 0, 1);
	out_Color = vec4(0.3f,0.0f,0.0f,1.0f) * shade;
	//out_Color = texture(texUnit,texCoord) * shade;
}
