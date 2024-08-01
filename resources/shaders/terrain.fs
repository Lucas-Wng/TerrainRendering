#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D roughMap;

void main()
{
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb;
    vec3 normalTexture = texture(normalMap, TexCoords).rgb;
    float roughness = texture(roughMap, TexCoords).r;

    normalTexture = normalTexture * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalTexture);

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Ambient
    vec3 ambient = 0.3 * diffuseColor * lightColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * lightColor * 0.7;

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0 * (1.0 - roughness)) * 0.2;
    vec3 specular = spec * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}