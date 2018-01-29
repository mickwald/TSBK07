#version 150

in  vec3 in_Position;
uniform mat4 myMatrix;
in vec3 in_vertcolor;
out vec3 frag_color;
vec4 transform;

void main(void)
{
	gl_Position = myMatrix * vec4(in_Position, 1.0);
	//out_color = vec3(0.0f,0.5f,0.0f);
	frag_color = in_vertcolor;
}
