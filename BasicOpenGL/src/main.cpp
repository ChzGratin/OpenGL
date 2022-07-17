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

    ShaderProgram mySP("../../shader/texture.vs", "../../shader/texture.fs", nullptr);
	mySP.use();
	mySP.setSampler("texture0", 0);
	
	float vert[] =
	{
		  0.5, 0.5, 0.0,  1.0, 1.0,
		 -0.5, 0.5, 0.0,  -1.0, 1.0,
		-0.5, -0.5, 0.0,  -1.0, -1.0,
		 0.5, -0.5, 0.0,  1.0, -1.0
	};
	unsigned int idx[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Image::setFlipVerticallyOnLoad(true);
	Image mollu("../../resource/bruaka_64.png", GL_TEXTURE_2D);
	Image hifumi("../../resource/hifumi_10.jpg", GL_TEXTURE_2D);

	//render loop
	while (!glfwWindowShouldClose(win))
	{
		//input

		//render background
		glClearColor(0.25f, 0.25f, 0.25, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hifumi.getImageID());

		//render object
		mySP.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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