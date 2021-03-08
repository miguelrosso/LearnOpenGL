#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "libraries/stb_image/stb_image.h"

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

Camera* mainCamera;
float deltaTime, lastFrame;
float lastMouseX, lastMouseY;
bool firstMouseInput = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera->ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseInput) 
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouseInput = false;
	}

	float xOffset = xpos - lastMouseX;
	float yOffset = ypos - lastMouseY;
	lastMouseX = xpos;
	lastMouseY = ypos;

	mainCamera->ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera->ProcessMouseScroll(yoffset);
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

	std::cout << "OGL context ready" << std::endl;

	glViewport(0, 0, 800, 600);

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	mainCamera = &camera;

	//GLFW callbacks have to be registered AFTER we create the window,
	//but BEFORE our game loop is initiated.
	//Frame Buffer Size change Callback (called upon window resize)
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	Shader ourShader("./shaders/shader.vert", "./shaders/shader.frag");
	Shader lightShader("./shaders/lightShader.vert", "./shaders/lightShader.frag");

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f) 
	};
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f) 
	};

	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);

	//Image Loading
	stbi_set_flip_vertically_on_load(true);

	std::string path = std::filesystem::absolute("./assets/backpack/backpack.obj").string();
	std::cout << "About to load file " << path << std::endl;
	//Backpack model
	//Model backpackModel(path.c_str());

	//Ligth mesh
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//Draw as wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);

	int nrAttrib;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttrib);
	std::cout << "Max nr of vertex attributes supported: " << nrAttrib << std::endl;

	ourShader.use();
	ourShader.setInt("material.diffuse", 0);
	ourShader.setInt("material.specular", 1);
	ourShader.setInt("material.emissive", 2);
	ourShader.setFloat("material.shininess", 64.0f);

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
	ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

	lightShader.use();
	lightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	lastMouseX = 400;
	lastMouseY = 300;

	glm::mat4 view = glm::mat4(1.0f);
	view = camera.GetViewMatrix();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

	deltaTime = lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		
		//Directional Light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.01f, 0.01f, 0.01f);
		ourShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
		ourShader.setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

		//Point Lights
		for (unsigned int i = 0; i < 4; i++) {
			const std::string pointLight = "pointLights[" + std::to_string(i) + "]";
			ourShader.setVec3(pointLight + ".position", pointLightPositions[i]);
			ourShader.setVec3(pointLight + ".ambient", glm::vec3(0.00f * i));
			ourShader.setVec3(pointLight + ".diffuse", glm::vec3(0.4f));
			ourShader.setVec3(pointLight + ".specular", glm::vec3(1.0f));
			ourShader.setFloat(pointLight + ".constant", 1.0f);
			ourShader.setFloat(pointLight + ".linear", 0.09f);
			ourShader.setFloat(pointLight + ".quadratic", 0.032f);
		}

		//Spot Light
		ourShader.setVec3("spotLight.position", camera.Position);
		ourShader.setVec3("spotLight.direction", camera.Front);
		ourShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
		ourShader.setVec3("spotLight.diffuse", glm::vec3(0.5f));
		ourShader.setVec3("spotLight.specular", glm::vec3(0.5f));
		ourShader.setFloat("spotLight.constant", 1.0f);
		ourShader.setFloat("spotLight.linear", 0.09f);
		ourShader.setFloat("spotLight.quadratic", 0.032f);
		ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		ourShader.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		
		//Render loaded mesh
		//backpackModel.Draw(ourShader);

		//Rendering Light Source
		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++) 
		{
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.2f));
			lightShader.setMat4("model", lightModel);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		glfwPollEvents();
		glfwSwapBuffers(window);

		std::cout << glGetError() << std::endl;
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	
	glfwTerminate();
	return 0;
}