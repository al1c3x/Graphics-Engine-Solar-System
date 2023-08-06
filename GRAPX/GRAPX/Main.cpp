#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h";
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"


int main()
{

	stbi_set_flip_vertically_on_load(true);

#pragma region Initialization
	//initialize glfw
	if (glfwInit() != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialized! \n");
		return -1;
	}

	// set opengl version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window 
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Joseph Santos", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to load window! \n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	//initialize glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
#pragma endregion


#pragma region Mesh Loading

	ObjData sun;
	LoadObjFile(&sun, "earth/earth.obj");
	GLfloat sunOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&sun,
		1.0f,
		sunOffsets,
		"sun"
	);

	ObjData moon;
	LoadObjFile(&moon, "earth/earth.obj");
	GLfloat moonOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&moon,
		1.0f,
		moonOffsets,
		"moon"
	);

	ObjData earth;
	LoadObjFile(&earth, "earth/earth.obj");
	GLfloat earthOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&earth,
		1.0f,
		earthOffsets,
		"earth"
	);

	std::vector<std::string> faces
	{
		"right.png",
		"left.png",
		"bottom.png",
		"top.png",
		"front.png",
		"back.png"
	};
	SkyBoxData skybox = loadSkyBox("Assets/skybox", faces);
#pragma endregion

#pragma region Shader Loading

	//load skybox shader program
	GLuint skyboxShaderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader");

	//load shader program
	GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_fragment.shader");
	glUseProgram(shaderProgram);

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "u_color");
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);


	// initialize MVP
	GLuint modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model");

	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");

	//initialize normal transformation
	GLuint normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal");
	GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos");
	GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color");
	glUniform3f(ambientColorLoc, 0.0f, 0.0f, 0.15f);

	glm::mat4 sunPlanet = glm::mat4(1.0f); // identity
	glm::mat4 moonPlanet = glm::mat4(1.0f); // identity
	glm::mat4 earthPlanet = glm::mat4(1.0f); // identity

	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(sunPlanet));
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(moonPlanet));
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(earthPlanet));

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//setup light shading
	//POINT LIGHT
	GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "u_light_pos");
	//DIRECTIONAL LIGHT
	GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir");
	glUniform3f(lightPosLoc, 1.0f, sunPlanet[3][1], sunPlanet[3][2]);
	glUniform3f(lightDirLoc, 0.0f, 1.0f, 1.0f);

	//flag for shading
	GLuint modelIdLoc = glGetUniformLocation(shaderProgram, "u_model_id");
	glUniform1f(modelIdLoc, 1.0f);


#pragma endregion

	// set bg color to green
	glClearColor(0.0f, 0.0f, 0.15f, 0.0f);

	// var for rotations
	  // var for rotations
	float xFactor = 0.0f;
	float xSpeed = 1.0f;
	float currentTime = glfwGetTime();
	float prevTime = 0.0f;
	float deltaTime = 0.0f;
	float rotFactor = 0.0f;


	//depth testing
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // set the depth test function

	//face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // set which face to cull
	//glFrontFace(GL_CCW); // set the front face orientation

	//std::cout << moon.textures[0] << std::endl;

	while (!glfwWindowShouldClose(window))
	{

#pragma region Viewport
		float ratio;
		int width, height;
		bool ortho = false;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
#pragma endregion

#pragma region Projection
		// Orthopgraphic projection but make units same as pixels. origin is lower left of window
		// projection = glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, 0.1f, 10.0f); // when using this scale objects really high at pixel unity size

		// Orthographic with stretching
		//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);

		// Orthographic with corection for stretching, resize window to see difference with previous example
		//projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 10.0f);

		
		projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 100.0f);


		// Perspective Projection
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.1f, 125.0f);
			
			ortho = true;
		}

			// Set projection matrix in shader
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));



#pragma endregion

#pragma region View
			glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
			glm::mat4 view = glm::lookAt(
				cameraPos,
				//glm::vec3(0.5f, 0.0f, -1.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),  //CENTER
				//trans[3][0], trans[3][1], trans[3][2]
				glm::vec3(0.0f, 1.0f, 0.0f)  //UP
			);
			glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			if (ortho == true) {
				cameraPos = glm::vec3(0.0f, 90.0f, -10.0f);
				glm::mat4 view = glm::lookAt(
					cameraPos,
					//glm::vec3(0.5f, 0.0f, -1.0f),
					glm::vec3(sunPlanet[3][0], sunPlanet[3][1], sunPlanet[3][2]),
					glm::vec3(0.0f, 1.0f, 0.0f)
				);
				
				glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
			}

			

	
		
#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		
		//DRAWING SKYBOX
		DrawSkybox(skybox, skyboxShaderProgram, view, projection);
		glUseProgram(shaderProgram);

		
		//draw SUN
		glUniform1f(modelIdLoc, 1.0f);
		glBindVertexArray(sun.vaoId);

		// transforms
		sunPlanet = glm::mat4(1.0f); // identity
		sunPlanet = glm::translate(sunPlanet, glm::vec3(5.0f, 0.0f, -7.0f)); // matrix * translate_matrix
		sunPlanet = glm::rotate(sunPlanet, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f));
		sunPlanet = glm::scale(sunPlanet, glm::vec3(1.0f, 1.0f, 1.0f));

		//send to shader
		glm::mat4 normalTrans = glm::transpose(glm::inverse(sunPlanet));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(sunPlanet));


		glActiveTexture(GL_TEXTURE0);
		GLuint sunTexture = sun.textures[sun.materials[1].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, sunTexture);
		//drawbackpack
		glDrawElements(GL_TRIANGLES, sun.numFaces, GL_UNSIGNED_INT, (void*)0);

		

		//draw MOON
		glUniform1f(modelIdLoc, 1.1f);
		glBindVertexArray(moon.vaoId);

		// transforms
		moonPlanet = glm::mat4(1.0f); // identity
		moonPlanet = glm::rotate(earthPlanet, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f));
		moonPlanet = glm::translate(moonPlanet, glm::vec3(5.0f, 0.0f, -7.0f));
		moonPlanet = glm::scale(moonPlanet, glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 normalTrans1 = glm::transpose(glm::inverse(moonPlanet));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans1));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(moonPlanet));

		GLuint moonTexture = moon.textures[moon.materials[2].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, moonTexture);
		glDrawElements(GL_TRIANGLES, moon.numFaces, GL_UNSIGNED_INT, (void*)0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
	


		//draw EARTH
		glBindVertexArray(earth.vaoId);

		// transforms
		earthPlanet = glm::mat4(1.0f); // identity
		earthPlanet = glm::rotate(sunPlanet, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f));
		earthPlanet = glm::translate(earthPlanet, glm::vec3(5.0f, 0.0f, -7.0f));
		earthPlanet = glm::scale(earthPlanet, glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 normalTrans2 = glm::transpose(glm::inverse(earthPlanet));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans2));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(earthPlanet));

		GLuint earthTexture = earth.textures[earth.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, earthTexture);
		glDrawElements(GL_TRIANGLES, earth.numFaces, GL_UNSIGNED_INT, (void*)0);
		

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		

		

		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		rotFactor += deltaTime * 30.0f;
		if (rotFactor > 360.0f) {
			rotFactor -= 360.0f;
		}
		prevTime = currentTime;


		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
	}
	return 0;
}