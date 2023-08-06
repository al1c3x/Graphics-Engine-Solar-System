#version 330 core
layout(location = 0) in vec3 a_vertex;

out vec3 texCoords;

//VIEW
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	texCoords = a_vertex;
	gl_Position = u_projection * u_view * vec4(a_vertex, 1.0);
}