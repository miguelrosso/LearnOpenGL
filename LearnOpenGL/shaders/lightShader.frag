#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
   //FragColor = vec4(ourColor, 1.0);
   //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
   FragColor = vec4(lightColor, 1.0);
}