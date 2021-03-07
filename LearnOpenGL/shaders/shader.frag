#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

struct Light {
    //vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;
uniform vec3 fragmentPos;

void main()
{
   vec3 ambient = light.ambient * (vec3(texture(material.diffuse, TexCoord)));

   vec3 norm = normalize(Normal);
   //vec3 lightDir = normalize(light.position - FragPos);
   vec3 lightDir = normalize(-light.direction);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * (diff * (vec3(texture(material.diffuse, TexCoord))));

   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = light.specular * (spec * (vec3(texture(material.specular, TexCoord))));

   vec3 emissive = vec3(texture(material.emissive, TexCoord)); 

   vec3 result = ambient + diffuse + specular + emissive;
   FragColor = vec4(result, 1.0);
}