#include <Shader.hpp>
#include <Image.hpp>

// #include <filesystem>

void framebuffersize_callback(GLFWwindow*, int, int);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* win = glfwCreateWindow(800, 600, "BasicOpenGL", nullptr, nullptr);
	if (!win)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(win, framebuffersize_callback);

    // current_path(): BasicOpenGL\\build\\Debug (or Release)
    // std::cout << std::filesystem::current_path() << std::endl;

    ShaderProgram mySP("../../shader/vertshader.vs", "../../shader/fragshader.fs");
	
	float vert[] = { 0.5, -0.5, 0.0,
					 0.0,  0.5, 0.0,
					-0.5, -0.5, 0.0};
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	Image myImg("../../resource/hifumi_10.jpg", GL_TEXTURE_2D);

	//render loop
	while (!glfwWindowShouldClose(win))
	{
		//input

		//render
		glClearColor(0.25f, 0.25f, 0.25, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//render object
		mySP.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//double buffering
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