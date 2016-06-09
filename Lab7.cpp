//Lab 6
//modified from http://learnopengl.com/
#include <ctime>
#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Building.h"
#include <..\COMP371_Lab7\objloader.hpp> 
#include "..\soil\SOIL.h"

using namespace std;

// Window dimensions
const GLuint WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
const GLfloat CAMERA_MOVEMENT_STEP = 2.20f;
const float ANGLE_ROTATION_STEP = 0.15f;

glm::vec3 camera_position;
glm::mat4 projection_matrix;

//Declaring global array
vector<glm::vec3> vecBuildingCoordinate;	//coordinate of the building
vector<glm::vec3> vecBuildingSize;			//building size
vector<glm::vec3> vecBuildingColor;		//building color
vector<glm::vec3> vecMapCoordinate;		//coordinate of the new maps (for procedural generator)

//Declaring the method signature
glm::vec3 createBuilding();
glm::vec3 createCoordinate();
glm::vec3 createColor();
float randomFloat(float a, float b);

float y_rotation_angle = 0.0f, x_rotation_angle = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	// Update the Projection matrix after a window resize event
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		camera_position.z += CAMERA_MOVEMENT_STEP;

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		camera_position.z -= CAMERA_MOVEMENT_STEP;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		camera_position.x -= CAMERA_MOVEMENT_STEP;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		camera_position.x += CAMERA_MOVEMENT_STEP;

	//rotate cube
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		x_rotation_angle += ANGLE_ROTATION_STEP;

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		x_rotation_angle -= ANGLE_ROTATION_STEP;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		y_rotation_angle += ANGLE_ROTATION_STEP;

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		y_rotation_angle -= ANGLE_ROTATION_STEP;
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);

		SOIL_free_image_data(image); //free resources
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

int main()
{ 
	srand(time(0)); //random number generator seed
	std::cout << "Starting Project Team 2" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cubemaps", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
	
	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;

	loadOBJ("cube.obj", vertices, normals, UVs);

	GLuint VAO, vertices_VBO, normals_VBO, UVs_VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertices_VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &UVs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");

	GLuint drawing_skybox_id = glGetUniformLocation(shaderProgram, "drawingSkybox");

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glActiveTexture(GL_TEXTURE0); //select texture unit 0

	GLuint cube_texture;
	glGenTextures(1, &cube_texture);
	glBindTexture(GL_TEXTURE_2D, cube_texture); //bind this texture to the currently bound texture unit

												// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load image, create texture and generate mipmaps
	int cube_texture_width, cube_texture_height;
	unsigned char* cube_image = SOIL_load_image("window2.jpg", &cube_texture_width, &cube_texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cube_texture_width, cube_texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, cube_image);

	SOIL_free_image_data(cube_image); //free resources
	
	unsigned char* top_image = SOIL_load_image("grass.jpg", &cube_texture_width, &cube_texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 10, GL_RGB, cube_texture_width, cube_texture_height, 10, GL_RGB, GL_UNSIGNED_BYTE, top_image);
	SOIL_free_image_data(top_image); //free resources


	//Setup our cubemap

	vector<const GLchar*> faces;
	faces.push_back("cube_map/a.jpg");	//right
	faces.push_back("cube_map/c.jpg");	// left
	faces.push_back("cube_map/top.jpg");	// top
	faces.push_back("cube_map/bottom.jpg");	// bottom
	faces.push_back("cube_map/b.jpg");	// back
	faces.push_back("cube_map/d.jpg");	// front

	glActiveTexture(GL_TEXTURE1);
	GLuint cubemapTexture = loadCubemap(faces);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	glUniform1i(glGetUniformLocation(shaderProgram, "cubeTexture"), 0); //cubeTexture should read from texture unit 0
	glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 1); //sky box should read from texture unit 1
	glUniform1i(glGetUniformLocation(shaderProgram, "grassTexture"), 10); //cubeTexture should read from texture unit 2

						
	
	//create 10 buildings
	for (int i = 0; i < 1000; i++) {
		createBuilding();
		createCoordinate();
		cout << "Create buildings " << endl;
		cout << "Map of Coordinate : " << vecMapCoordinate.size() << endl;
		cout << "Number of build : " << vecBuildingSize.size() << endl;
		if (i == 10) {
			vecBuildingCoordinate.push_back(glm::vec3(0, -1, 0));
			vecBuildingSize.push_back(glm::vec3(100, 0.5, 100));
		}
	}
	

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model_matrix;
		model_matrix = glm::rotate(model_matrix, y_rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
		model_matrix = glm::rotate(model_matrix, x_rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));

		glm::mat4 view_matrix;
		view_matrix = translate(view_matrix, camera_position);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));			// Model
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));		// The pespective
		glBindVertexArray(VAO);

		//Draw the skybox

		glm::mat4 skybox_view = glm::mat4(glm::mat3(view_matrix)); //remove the translation data
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(skybox_view));

		glDepthMask(GL_FALSE);

		glUniform1i(drawing_skybox_id, 1); //set the uniform boolean
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDepthMask(GL_TRUE);

		//Draw the textured cube

		// Plane and one block
		
			
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
				glUniform1i(drawing_skybox_id, 0); //set the boolean
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			
			
		
		//glBindVertexArray(VAO);
		//cout << vecMapCoordinate.size() << endl;
				glm::mat4 model_building;
		for (GLuint i = 0; i < vecBuildingCoordinate.size(); i++) {
			//cout << "im currentoy to draw " << vecBuildingCoordinate.size() << " cubes. " << endl;
			
			model_matrix = glm::translate(model_matrix, vecBuildingCoordinate[i]);
			model_matrix = glm::scale(model_matrix, vecBuildingSize[i]);
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
			//glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		//	glUniform1i(drawing_skybox_id, 0); //set the boolean
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		
		}

		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
/*
Method : CreateBuilding()
Return a random size of building
*/
glm::vec3 createBuilding() {
	
	float width = randomFloat(1, 2);	// x
	float height = randomFloat(1,2);	// y
	float depth = randomFloat(1,2);		// z

	cout << "WIDTH = " << width << endl;
	cout << "depth = " << depth << endl;
	cout << "height = " << height << endl;
	
	glm::vec3 sizeScale = glm::vec3(width, height, depth);
	vecBuildingSize.push_back(sizeScale);
	
	return sizeScale;

}
glm::vec3 createColor() {
	
	float R = rand() % 4;
	float G = rand() % 4;
	float B = rand() % 4;

	cout << "R = " << R << endl;
	cout << "G = " << G << endl;
	cout << "B = " << B << endl;
	
	vecBuildingColor.push_back(glm::vec3(R, G, B));
	glm::vec3 color = glm::vec3(R, G, B);
	return color;
}

glm::vec3 createCoordinate() {
	
	float coox = randomFloat(-50,50);
	float cooz = randomFloat(-50, 50);
	cout << "coox = " << coox << endl;
	cout << "cooz = " << cooz << endl;

	glm::vec3 coordinate = glm::vec3(coox, 0, cooz);
	vecBuildingCoordinate.push_back(coordinate);
	return coordinate;

}
float randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}