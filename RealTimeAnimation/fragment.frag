#version 330 core
in vec3 inputColor;
in vec4 posColor;
out vec4 color;
in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
//uniform vec4 ourColor;
void main()
{
	vec2 newTextCoord = vec2(TexCoord.x, 1.0f - TexCoord.y);
	color = mix(texture(ourTexture1, newTextCoord), texture(ourTexture2, newTextCoord), 0.2);   

}