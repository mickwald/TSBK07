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
	const float kd=0.45f;
	const float ks=0.5f;
	float l_spec = 0;
	const float l_l = 0.9f;
	const float l_a = 0.1f;
	vec3 shade;
	float l_diff = 0;
	float l_amb = 0;
	vec3 l_spec_arr = vec3(0,0,0);
	vec3 l_diff_arr = vec3(0,0,0);
	vec3 s,r,v;
	
	//ambient
	l_amb = kd*l_a;

	int i;
	for (i = 0; i <= 3; i++){
		if(!isDirectional[i]){
			s = -(normalize(lightSourcesDirPosArr[i] - vec3(worldViewPos)));  //View
			
		} else {
			s = -(normalize(lightSourcesDirPosArr[i])); //View
		}
		if(dot(s, fragNormal) > 0.0){	//View
		r = normalize(reflect(s, normalize(fragNormal)));	//View
		v = normalize(vec3(worldViewPos));	//View
		l_spec = ks * l_l * pow(max(0.01f,dot(r,v)),specularExponent[i]);
		l_spec_arr += l_spec * lightSourcesColorArr[i];
		l_diff = kd * l_l * max(0,dot(s, normalize(fragNormal)));	//View
		l_diff_arr += l_diff * lightSourcesColorArr[i];
		}
	} 
	
	shade = clamp(clamp(l_diff_arr,0,1)+clamp(l_spec_arr,0,1), 0, 1);
	//shade = l_diff_arr + l_spec_arr;
	shade += clamp(l_amb,0,1);
	out_Color = vec4(shade[0],shade[1],shade[2],1.0f);
	//out_Color = texture(texUnit,texCoord) * shade;
}
