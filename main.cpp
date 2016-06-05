#include <iostream>

//GLEW
#define GLEW_STATIC
#include <G:\My Documents\comp 371\assgt2\glew-1.13.0-win32\glew-1.13.0\include\GL\glew.h>

//GLFW
#include <G:\My Documents\comp 371\assgt2\glfw-3.1.2.bin.WIN32\glfw-3.1.2.bin.WIN32\include\GLFW\glfw3.h>

// GLM
#include <G:\My Documents\comp 371\assgt2\glm-0.9.6.3\glm\glm\gtc\matrix_transform.hpp>
#include <G:\My Documents\comp 371\assgt2\glm-0.9.6.3\glm\glm\gtc\type_ptr.hpp>
#include <G:\My Documents\comp 371\assgt2\glm-0.9.6.3\glm\glm\glm.hpp>

#include <G:\My Documents\comp 371\assgt2\assgt2\Shader.h>

//set my window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;


//set callback functions here
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos,double ypos);
//certain useful variables
double xpos, ypos;
int numOfPoints;
int pointsLeft;
int currentIndex;
bool keys[1024];
GLfloat fov = 45.0f;
glm::vec3 arrayCoordinate[25];
glm::vec3 bezierCoordinate[800];
bool draw = false;

//set certain functions
float bezierX(double p1x, double p2x, double p3x, double p4x, double t);
float bezierY(double p1y, double p2y, double p3y, double p4y, double t);
void drawBezier(GLfloat controlPoints[], int j);
int askForPoints();
GLfloat bezierCurve(float t, GLfloat P0, GLfloat P1, GLfloat P2, GLfloat P3);
void bezierfunction(glm::vec3 array[]);

GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


//CAMERA SETUP
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//main program
int main()
{
	pointsLeft = askForPoints();
	numOfPoints = pointsLeft;
	currentIndex = 0;
	//GLFW init
	glfwInit();

	//All required options to make it work
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	//double buffering here
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	//make window here
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 2", nullptr, nullptr);
	glfwMakeContextCurrent(window);


	// Set the required callback functions
	//keys
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);


	//Shader
	Shader ourShader("<G:\My Documents\comp 371\assgt2\assgt2\Shader.vs>","<G:\My Documents\comp 371\assgt2\assgt2\Shader.frag>");
	//use this do draw the points after
	GLfloat vertices[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	//make the arrays
	GLuint VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0); // specifies how vertex array will be processed.
	glEnableVertexAttribArray(0); // enables the setting above 

	// Color attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); //specifies what position in data is refered to for color.
	glEnableVertexAttribArray(2); // enables the setting above.

	glBindVertexArray(0); // Unbinds the VAO
	glPointSize(4.5f);

	//loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwGetCursorPos(window, &xpos, &ypos); 

		glClearColor(0.5f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.Use();

		glm::mat4 view;
		glm::mat4 projection;
		projection = glm::ortho(0.0f, (GLfloat)WIDTH, (GLfloat)HEIGHT, 0.0f, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//get locations
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");


		//pass to shaders
		
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw points
		glBindVertexArray(VAO);
		if (draw == false)
		{
			for (GLuint j = 0; j < numOfPoints; j++)
			{
				glm::mat4 model;
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				model = glm::translate(model, arrayCoordinate[j]);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_POINTS, 0, 1);
			}
		}
		else
		{
			for (int i = 0; i < sizeof(bezierCoordinate); i++)
			{
				glm::mat4 model;
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				model = glm::translate(model, bezierCoordinate[i]);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_POINTS, 0, 1);
			}
		}
		
		

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	//clean up memory
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

int askForPoints() 
{
	bool done = false;
	while (!done)
	{
		std::cout << "Please enter a number of points (N) for the Bezier Splines"<< std::endl;
		std::cin >> numOfPoints;
		if (numOfPoints <= 3)
		{
			std::cout << "The number you entered is not valid, please enter another number of points (N) for the bezier Splines (greater than 3)" << std::endl;
		}
		else
		{
			std::cout << "Your number of points (N) is :" << numOfPoints<< std::endl;
			done = true;
		}
	}
	return numOfPoints;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//escape closes window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS) //draw spline as line
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		bezierfunction(arrayCoordinate);
		/*glPointSize(5.0);
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < sizeof(bezierCoordinate); i++){
			GLfloat holder[1][3];
			holder[0][0] = bezierCoordinate[i].x;
			holder[0][1] = bezierCoordinate[i].y;
			holder[0][2] = bezierCoordinate[i].z;
			glVertex3fv(holder[0]);
		}
		glEnd();
		glFlush();*/
		draw = true;
	}
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)// draw spline as fill
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (key == GLFW_KEY_P && action == GLFW_PRESS) // draw spline as points
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (pointsLeft != 0)
		{
			//double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			std::cout << "X :" << xpos << std::endl;
			std::cout << "Y :" << ypos << std::endl;
			//save them in an array
			arrayCoordinate[currentIndex].x = (GLfloat)xpos;
			arrayCoordinate[currentIndex].y = (GLfloat)ypos;//to fix inverted Y axis, do HEIGHT - ypos
			arrayCoordinate[currentIndex].z = 0.0f;
			currentIndex++;
			pointsLeft--;
			std::cout << "You have " << pointsLeft << " points left to chose." << std::endl;
		}
	}
}


//B(t) = (1-t^3)P0 + 3(1-t)^2P1 + 3(1-t)t^2P2 + t^3P3
float bezierX(double p1x, double p2x, double p3x, double p4x, double t)
{
	float answer;
	answer = (pow((1-t),3.0)*p1x)+(3*pow((1-t),2)*t*p2x)+(3*(1-t)*t*t*p3x)+(pow(t,3)*p4x);
	return answer;
}

float bezierY(double p1y, double p2y, double p3y, double p4y, double t)
{
	float answer;
	answer = (pow((1 - t), 3.0)*p1y) + (3 * pow((1 - t), 2)*t*p2y) + (3 * (1 - t)*t*t*p3y) + (pow(t, 3)*p4y);
	return answer;
}
void drawBezier(GLfloat controlPoints[], int j)
{
	double t;
	float x;
	float y;
	for (int i = 1; i <= 40; i++)//40 is the number of segments we want in each one of our bezier splines
	{
		//go through T values
		t = i / 40.0;
		//calculate x coordinate and y coordinate
		x=bezierX(controlPoints[0], controlPoints[2], controlPoints[4], controlPoints[6],t);
		y=bezierY(controlPoints[1], controlPoints[3], controlPoints[5], controlPoints[7],t);
		//now save x and y into a new array, which only holds values of points on the spline
		bezierCoordinate[i-1+j*40].x = x;
		bezierCoordinate[i-1+j*40].y = y;
		bezierCoordinate[i-1+j*40].z = 0.0f;
		//DRAW THE bezierCoordinate array now

	}

}
void bezierfunction(glm::vec3 array[])
{
	int splineNum;
	GLfloat controlPoints[8];
	splineNum = ((numOfPoints - 4) / 3) + 1;//number of seperate bezier splines we will print
	std::cout << "You will have " << splineNum << " bezier splines." << std::endl;
	for (int i = 0; i < splineNum; i++)//for each of these splines, save 4 clicked points (in the right order)
	{
		controlPoints[0] = (GLfloat)array[i*3].x;
		controlPoints[1] = (GLfloat)array[i * 3].y;

		controlPoints[2] = (GLfloat)array[i * 3 + 1].x;
		controlPoints[3] = (GLfloat)array[i * 3 + 1].y;

		controlPoints[4] = (GLfloat)array[i * 3 + 2].x;
		controlPoints[5] = (GLfloat)array[i * 3 + 2].y;

		controlPoints[6] = (GLfloat)array[i * 3 + 3].x;
		controlPoints[7] = (GLfloat)array[i * 3 + 3].y;

		drawBezier(controlPoints, i);
	}
}

