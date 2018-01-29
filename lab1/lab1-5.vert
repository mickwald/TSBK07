#version 150

in  vec3 in_Position;
uniform mat4 myMatrix;
in vec3 in_vertcolor;
out vec3 frag_color;
uniform mat4 Rotx;
uniform mat4 Roty;
uniform mat4 Rotz;

void main(void)
{
	mat4 transform = Rotz * Rotx;
	gl_Position = vec4(in_Position, 1.0) * transform;
	//out_color = vec3(0.0f,0.5f,0.0f);
	frag_color = in_vertcolor;
}
