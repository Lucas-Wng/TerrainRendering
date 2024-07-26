#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float Height;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D diffuseMap;
uniform sampler2D dispMap;
uniform sampler2D normalMap;
uniform sampler2D roughMap;

void main()
{
    // Ambient lighting
    vec3 ambient = 0.1 * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine lighting components
    vec3 lighting = ambient + diffuse + specular;

    // Fetch texture maps
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb;
    vec3 displacement = texture(dispMap, TexCoords).rgb;
    vec3 normalTexture = texture(normalMap, TexCoords).rgb;
    vec3 roughness = texture(roughMap, TexCoords).rgb;

    // Adjust normal with normal map
    norm = normalize(norm * normalTexture);

    // Combine all components
    vec3 result = lighting * (diffuseColor * 0.8 + displacement * 0.1 + roughness * 0.1);
    result = mix(result, lighting * diffuseColor, (Height - 0.2) * 2.0);

    FragColor = vec4(result, 1.0);
}