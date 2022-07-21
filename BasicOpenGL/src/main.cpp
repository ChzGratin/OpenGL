#include <Shader.hpp>
#include <Image.hpp>
#include <Mesh.hpp>
#include <Model.hpp>

// #include <filesystem>

void framebuffersize_callback(GLFWwindow*, int, int);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* win = glfwCreateWindow(600, 600, "BasicOpenGL", nullptr, nullptr);
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

	glViewport(0, 0, 600, 600);
	glfwSetFramebufferSizeCallback(win, framebuffersize_callback);

    // current_path(): BasicOpenGL\\build\\Debug (or Release)
    // std::cout << std::filesystem::current_path() << std::endl;

	ShaderProgram sp1("../../shader/mesh.vs", "../../shader/mesh.fs", nullptr);
	Image::setFlipVerticallyOnLoad(true);
	Model m1("../../resource/model/model.obj");

	//render loop
	//glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(win))
	{
		//input

		//render background
		glClearColor(0.25f, 0.25f, 0.25, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//render objects
		sp1.use();
		m1.draw(sp1);

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