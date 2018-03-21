#version 150

in vec2 texCoord;
in vec3 fragNormal;
out vec4 outColor;
uniform sampler2D tex;
uniform bool color;
void main(void)
{
	const vec3 light = vec3 (0.58,0.58,0.58);
	const float kd=0.95;
	float shade;	
	shade = kd * dot(normalize(fragNormal), light);
	shade = clamp(shade, 0, 1);
	if(color){
		outColor = vec4(0.0f,0.0f, 0.0f,1.0f);
	} else {
		outColor = texture(tex, texCoord) * shade;
	}
}
