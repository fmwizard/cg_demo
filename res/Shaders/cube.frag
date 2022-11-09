#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{    
    // ambient
    vec3 ambient = material.ambient * light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;
            
	// specular
	vec3 viewDir = normalize(viewPos - Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * material.specular) * light.specular;

	vec3 lighting = ambient + diffuse + specular;

    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(skybox, R).rgb * lighting, 1.0);
}