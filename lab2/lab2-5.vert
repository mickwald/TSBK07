#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 inTexCoord;
uniform mat4 uniTransform;
uniform float uniTime;
uniform mat4 uniProjection, uniCamera;
uniform mat4 uniTotal;
out vec3 frag_color;
out vec2 texCoord;
out vec3 fragNormal;


void main(void)
{
	fragNormal = in_Normal;
	gl_Position = uniProjection * uniCamera * uniTotal * uniTransform * vec4(in_Position, 1.0);
	vec3 normal = mat3(uniProjection * uniCamera * uniTotal * uniTransform) * in_Normal;
	frag_color = vec3(0.5f,1.0f,0.8f)- normal;
	texCoord = inTexCoord;
}
