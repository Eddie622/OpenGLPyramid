/*
 * Pyramid.cpp
 *
 *  Created on: Jul 23, 2020
 *      Author: Admin
 */

/* Header Inclusions */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>


// GLM Math Header inclusions
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std; // Standard namespace

#define WINDOW_TITLE "Pyramid"

/* Shader program macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Var declaration for shader, window size, initialization, buffer and array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, EBO, texture;

// Function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);

// Vertex shader source
const GLchar* vertexShaderSource = GLSL(330,

	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrrib Pointer 0
	layout(location = 1) in vec3 color; // Color data from Vertex Attrib Pointer 1

	out vec3 mobileColor; // variable to transfer color data to the fragment shader

	// Global vars for transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() {
		gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertex data using matrix
		mobileColor = color; // references incoming color data
	}
);

// Fragment shader source
const GLchar* fragmentShaderSource = GLSL(330,

	in vec3 mobileColor; // variable to hold incoming color data from vertex shader

	out vec4 gpuColor; // variable to pass color data to the GPU

	void main() {
		gpuColor = vec4(mobileColor, 1.0); // Sends color data to the GPU rendering
	}
);

/* Main Program */
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	UCreateShader();

	UCreateBuffers();

	// User the shader program
	glUseProgram(shaderProgram);

	// Set background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutDisplayFunc(URenderGraphics);

	// Create and run OpenGL Loop
	glutMainLoop();

	// Deletes buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return 0;
}

// Resize window to fit primitives
void UResizeWindow(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

// Render Graphics
void URenderGraphics(void)
{
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	// Activate the vertex array object before rendering and transforming them
	glBindVertexArray(VAO);

	// Declare a 4x4 identity matrix uniform variable to handle transformations
	glm::mat4 model(1.0f);

	// Place the object at the center of the viewport
	model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));

	// Rotate the object 15 degrees on the x-axis
	model = glm::rotate(model, -270.0f, glm::vec3(1.0, 0.0f, 0.0f));

	// Rotate the object 45 degrees on the Y-axis
	model = glm::rotate(model, 45.0f, glm::vec3(0.0, 1.0f, 0.0f));

	// Increase the object size by a scale of 2
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	// transforms the camera and set the
	// I also did not set the view using the (1.0f) param
	glm::mat4 view(1.0f);
	view = glm::translate(view, glm::vec3(0.5f, 0.0f, -5.0f));

	// Perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Retrieves and passes transform matirices to the shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // Deactivate the Vertex Array Object

	glutSwapBuffers(); // Flips the back buffer with the front buffer every frame

}

// Creates the shader program
void UCreateShader()
{
	// Vertex shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Fragment shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);;
	glLinkProgram(shaderProgram);

	// Delete the vertex and fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// creates the buffer and array objects
// One such issue I had was the position of the vertices which where
// Arranged in such a way as the model was flat
void UCreateBuffers()
{

	// Position and color data
	GLfloat verticies[] = {

		// Vertex Positions    // Color Data			// Vertex position
		-0.5f, -0.5f,  0.0f,	1.0, 1.0f, 0.0f,		// 0
		0.0f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		// 1
		0.5f, -0.5f,  0.0f,		1.0f, 0.0f, 1.0f,		// 2
		-0.5f, -0.5f, -1.0f,	1.0f, 0.0f, 1.0f,		// 3
		0.5f, -0.5f, -1.0f,		0.5f, 0.5f, 1.0f,		// 4
	};

	// Index data to share position data
	GLuint indices[] = {

							0, 1, 2,	// Front Triangle
							0, 3, 1,	// Right Side triangle
							3, 1, 4,	// Back triangle
							4, 1, 2,	// Left back side triangle
							0, 3, 4,	// Bottom Triangle
							0, 4, 2		// Bottom triangle

	};

	// Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Activate the vertex array object before binding and setting any VBOs or vertex attribute pointers
	glBindVertexArray(VAO);

	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	// Activate the element buffer object / indicies
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// set attribute pointer 0 to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set attribute pointer 1 to hold Color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Sets polygon mode allows me to see wireframe view
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(0);
}

