#version 330 core
uniform vec3 u_color;
out vec4 FragColor;
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform vec3 u_light_dir;
uniform vec3 u_camera_pos;
uniform vec3 u_ambient_color; 


void main()
{
	//vertexColor = vec3(1.0, 1.0, 1.0);
	vec3 lightVector = normalize(u_light_dir);

	//float distance = length(u_light_pos - FragPos);
	//float gradient = attenuate(distance, 30.0);

	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float specularStrength = 1.5;

	vec3 viewDir = normalize(u_camera_pos - FragPos);
	vec3 reflectDir = reflect(-lightVector, Normal);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 4);

	vec3 specular = specularStrength * spec * lightColor;

	vec3 diffuse = vec3(max(dot(Normal, lightVector), 0.0)) * lightColor;
	vec3 ambient = u_ambient_color * lightColor;



	FragColor = vec4(ambient + diffuse + specular, 1.0) * texture(texture_diffuse, UV);
	//FragColor = texture(texture_diffuse, UV);
	//FragColor = vec3(u_color);
}

