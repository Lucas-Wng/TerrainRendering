#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D dispMap;

void main()
{
    TexCoords = aTexCoords;

    float displacement = texture(dispMap, aTexCoords).r * 0.04;
    vec3 displacedPos = aPos + aNormal * displacement;

    vec3 bitangent = normalize(cross(aNormal, aTangent));
    vec3 tangent = normalize(aTangent);
    vec3 normal = normalize(aNormal);
    TBN = mat3(tangent, bitangent, normal);

    FragPos = vec3(model * vec4(displacedPos, 1.0)); // Use displacedPos here
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);

    gl_Position = projection * view * model * vec4(displacedPos, 1.0);
}