#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_UV;
//UNIFORM TRANSFORMS
//uniform mat4 u_transform; 
//PROJECTION
uniform mat4 u_model2;
//VIEW
uniform mat4 u_view2;
uniform mat4 u_projection2;

out vec2 UV;

void main()
{
	//UNIFORM TRANSFORMS
	//gl_Position = u_transform * vec4(a_pos, 1.0); 

	//PROJECTION 
	//gl_Position = u_projection * u_model * vec4(a_pos, 1.0);

	//VIEW
	gl_Position = u_projection2 * u_view2 * u_model2 * vec4(a_pos, 1.0);
	UV = a_UV;
}