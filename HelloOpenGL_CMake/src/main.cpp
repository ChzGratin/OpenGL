//====Reference====
//Source Code: Joey de Vries - Learn OpenGL

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//identifier "WINDOW_WIDTH" is undefined
//identifier "WINDOW_HEIGHT" is undefined
//identifier "WINDOW_NAME" is undefined

void framebuffersize_callback(GLFWwindow*, int, int);

int main()
{
	SPDLOG_INFO("main()");

	SPDLOG_INFO("glfwInit()");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	SPDLOG_INFO("glfwCreateWindow()");
	GLFWwindow* win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
	if (!win)
	{
		SPDLOG_ERROR("FAIL: glfwCreateWindow()");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		SPDLOG_ERROR("FAIL: gladLoadGLLoader");
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(win, framebuffersize_callback);

	//render loop
	SPDLOG_INFO("render loop");
	while (!glfwWindowShouldClose(win))
	{
		//input

		//render
		glClearColor(0.25f, 0.25f, 0.25, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	SPDLOG_INFO("glfwTerminate()");
	glfwTerminate();

	SPDLOG_INFO("main(): return 0;");
	return 0;
}

void framebuffersize_callback(GLFWwindow* pWin, int w, int h)
{
	SPDLOG_INFO("framebuffersize_callback()");
	glViewport(0, 0, w, h);
}