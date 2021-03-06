#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "libraries/stb_image/stb_image.h"

#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main() {
	if (glfwInit() == GL_FALSE) 
	{
		std::cout << "Error Initializing GLFW" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (!window) 
	{
		std::cout << "Error Creating GLFW window object" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to Initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);


	//GLFW callbacks have to be registered AFTER we create the window,
	//but BEFORE our game loop is initiated.
	//Frame Buffer Size change Callback (called upon window resize)
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	//Vertex Shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (vertexShader == 0)
	{
		std::cout << "Error Creating Vertex Shader" << std::endl;
		glfwTerminate();
		return -1;
	}

	Shader ourShader("./shaders/shader.vert", "./shaders/shader.frag");

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
		0, 1, 3,  // first triangle
		1, 2, 3   // second triangle
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//First param is attrib location (in this case, it is specified in the shader with 
	//(location = 0))
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Vertex Array Objects contain state for:
	glBindVertexArray(VAO);
		//- Vertex Buffer Objects
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//- Element Buffer Objects
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//- Attribs & Uniforms
		//Position Attrib
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//Color Attrib
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(1);
		//TexCoord Attrib
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//Image Loading
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("./assets/container.jpg", &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
		glfwTerminate();
		return -1;
	}

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	data = stbi_load("./assets/awesomeface.png", &width, &height, &nrChannels, 0);
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	//Draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	int nrAttrib;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttrib);
	std::cout << "Max nr of vertex attributes supported: " << nrAttrib << std::endl;

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(50.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();

		int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}