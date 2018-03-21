#version 150

in vec2 texCoord;
in vec3 fragNormal;
in vec3 Pos;
in vec4 worldViewPos;

out vec4 outColor;

uniform sampler2D texGrass;
uniform sampler2D texTerrain;
uniform sampler2D texMountain;
uniform sampler2D texLake;
uniform sampler2D texSphere;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

uniform bool color;
void main(void)
{
	const float kd=0.45f;
	const float ks=0.5f;
	const float l_l = 0.9f;
	const float l_a = 0.9f;
	vec3 shade;

	float l_spec = 0;
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
	shade += clamp(l_amb,0,1);

	const vec3 light = vec3 (0.58,0.58,0.58);
	const float kdNoSpec=0.95;
	float shadeNoSpec;
	shadeNoSpec = kdNoSpec * dot(normalize(fragNormal), light);
	shadeNoSpec = clamp(shadeNoSpec, 0, 1);


	if(color){
		outColor = texture(texSphere, texCoord)*vec4(shade[0],shade[1],shade[2],1.0f);
	} else {
			if(Pos.y == 0){ // Apply Lake texture Rock texture * blue
				outColor = texture(texLake, texCoord) * vec4(shade[0],shade[1],shade[2],1.0f) * vec4(0.0f, 0.6f, 1.0f, 1.0f);
			}
			else if(Pos.y > 0.0f && Pos.y <= 0.3f){	//Beach
				outColor = texture(texLake, texCoord) * vec4(shade[0],shade[1],shade[2],1.0f) * vec4(1.0f, 1.0f, 0.5f, 1.0f);
			}
		 	else if(Pos.y >= 8.0f && Pos.y < 9.7f){ //Apply Mountain texture
				outColor = texture(texMountain, texCoord) * vec4(shade[0],shade[1],shade[2],1.0f);
			}
			else if(Pos.y >= 9.0f){ // Apply snow to mountain peaks
 				outColor =  vec4(shade[0],shade[1],shade[2],1.0f)* vec4(1.0f, 1.0f, 1.0f, 1.0f);
 			}
			else{
				outColor = texture(texGrass, texCoord) * shadeNoSpec;
				//outColor = vec4(shade[0],shade[1],shade[2],1.0f);
			}
	}
}
