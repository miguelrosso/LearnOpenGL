#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform float gamma;
uniform sampler2D screenTexture;

void main() 
{
	vec3 fragColor = texture(screenTexture, TexCoords).rgb;

	//Gamma correction
	FragColor = vec4(pow(fragColor.rgb, vec3(1.0/gamma)), 1.0);
}
