#version 150

out vec4 outColor;
in vec3 exNormal;
in vec2 texCoord;
uniform sampler2D grass, conc, dirt, redsquares, water, map;
uniform int opt;

void main(void)
{
	const vec3 light = vec3(0.58, 0.58, 0.58);
	float shade;
	
	shade = dot(normalize(exNormal), light) + 0.3;

	if (opt == 1)
	{
		vec4 m = texture(map, texCoord);
		float mmag = max(0, 1 - m.r - m.g - m.b);
		outColor = shade *
				(m.a*(texture(grass, texCoord) * m.r +
				texture(conc, texCoord) * m.g +
				texture(dirt, texCoord) * m.b +
				texture(redsquares, texCoord) * mmag)
				+ texture(water, texCoord) * (1-m.a));
	}
	else
	{
		outColor = shade * texture(map, texCoord);
	}
}
