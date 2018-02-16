#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 inTexCoord;

uniform vec4 inColor;
uniform mat4 uniProjection;
uniform float uniTime;
uniform mat4 uniTotal;
uniform mat4 uniWorld;

out vec2 texCoord;
out vec3 fragNormal;
out vec4 fragColor;
out vec4 worldViewPos;


void main(void)
{
	fragColor = inColor;	
	gl_Position = uniProjection * uniTotal * vec4(in_Position, 1.0); //Projection coords
	vec3 normal = mat3(uniTotal) * in_Normal;	//View Coords
	fragNormal = normal;	//View Coords
	texCoord = inTexCoord;	//texCoords
	worldViewPos = uniTotal * vec4(in_Position, 1.0);	//View Coords
}
