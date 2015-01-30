
uniform mat4 mvp;

vec4 transform(vec4 position)
{
	return mvp * position;
}