#version 150

in  vec3 in_Position;
uniform mat4 myMatrix;
vec4 transform;

void main(void)
{
	transform = myMatrix * vec4(in_Position, 1.0);
	gl_Position = transform;
}
