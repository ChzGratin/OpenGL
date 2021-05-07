#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffersize_callback(GLFWwindow*, int, int);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* win = glfwCreateWindow(800, 600, "Hello, OpenGL", nullptr, nullptr);
	if (!win)
	{
		std::cout << "FAIL: glfwCreateWindow()\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "FAIL: gladLoadGLLoader\n";
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(win, framebuffersize_callback);

	//render loop
	while (!glfwWindowShouldClose(win))
	{
		//input

		//render
		glClearColor(0.25f, 0.25f, 0.25, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffersize_callback(GLFWwindow* pWin, int w, int h)
{
	glViewport(0, 0, w, h);
}