#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_emissive1;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

#define NR_POINT_LIGHTS 4
uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;
uniform vec3 fragmentPos;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //Directional light
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    //Point Lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) 
        result += CalcPointLight(pointLights[i], norm, viewDir);
    
    //Spot light
    result += CalcSpotLight(spotLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) 
{
    vec3 lightDir = normalize(-light.direction);
    
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    //specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_diffuse1, TexCoord));
    return (ambient + diffuse + specular);
};

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    //specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //Blinn-Phong specular model
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    //attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                                light.quadratic * (distance * distance));

    //combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord)); 
    vec3 specular = light.specular * spec * vec3(texture(material.texture_diffuse1, TexCoord));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
};

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - FragPos);

    //ambient shading
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord ));

    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));

    //specular shading
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));

    //spotlight with soft edges
	float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    //attenuation
	float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation; 

    return (ambient + diffuse + specular);
};
