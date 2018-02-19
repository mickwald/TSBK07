#version 150

in vec2 texCoord;
in vec3 fragNormal;
in vec3 Pos;

out vec4 outColor;

uniform sampler2D texGrass;
uniform sampler2D texTerrain;
uniform sampler2D texMountain;
uniform sampler2D texLake;
uniform sampler2D texSphere;

uniform bool color;
void main(void)
{
	const vec3 light = vec3 (0.58,0.58,0.58);
	const float kd=0.95;
	float shade;
	shade = kd * dot(normalize(fragNormal), light);
	shade = clamp(shade, 0, 1);
	if(color){
		outColor = texture(texSphere, texCoord);
	} else {
			if(Pos.y == 0){ // Apply Lake texture Rock texture * blue
				outColor = texture(texLake, texCoord) * shade * vec4(0.0f, 0.6f, 1.0f, 1.0f);
			}
			else if(Pos.y > 0.0f && Pos.y <= 0.3f){	//Beach
				outColor = texture(texLake, texCoord) * shade * vec4(1.0f, 1.0f, 0.5f, 1.0f);
			}
		 	else if(Pos.y >= 8.0f && Pos.y < 9.7f){ //Apply Mountain texture
				outColor = texture(texMountain, texCoord) * shade;
			}
			else if(Pos.y >= 9.0f){ // Apply snow to mountain peaks
 				outColor =  shade* vec4(1.0f, 1.0f, 1.0f, 1.0f);
 			}
			else{
				outColor = texture(texGrass, texCoord) * shade;
			}
	}
}
