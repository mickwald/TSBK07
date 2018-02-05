#version 150

out vec4 out_Color;

in vec4 worldViewPos;
in vec2 texCoord;
in vec3 fragNormal;
in vec4 fragColor;

uniform vec4 color;
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];
uniform sampler2D texUnit;

void main(void)
{
	const float kd=0.95f;
	const float ks=0.5f;
	float l_spec = 0;
	const float l_l = 0.9f;
	const float l_a = 0.1f;
	float shade = 0;
	float l_diff =0;
	float l_amb = 0;
	
	//ambient
	l_amb = kd*l_a;

	int i;
	for (i = 0; i <= 3; i++){
		vec3 s = -(normalize(lightSourcesDirPosArr[i] - vec3(worldViewPos)));
		vec3 r = normalize(reflect(s, normalize(fragNormal)));
		vec3 v = normalize(vec3(worldViewPos));
	
		
		l_diff = kd * l_l * max(0,dot(s, normalize(fragNormal)));
		l_spec += ks * l_l * pow(max(0,dot(r,v)),specularExponent[i]);	
	} 

	shade = clamp(clamp(l_diff,0,1)+clamp(l_amb,0,1)+clamp(l_spec,0,1), 0, 1);
	out_Color = vec4(0.8f,0.8f,0.8f,1.0f) * shade;
	//out_Color = texture(texUnit,texCoord) * shade;
}
