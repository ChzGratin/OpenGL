#include <Shader.hpp>
#include <Image.hpp>
#include <Mesh.hpp>
//#include <Model.hpp>

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

	ShaderProgram basicSP("../../shader/mesh.vs", "../../shader/mesh.fs", nullptr);

	Image::setFlipVerticallyOnLoad(true);
	Image hifumi("../../resource/hifumi_10.jpg", GL_TEXTURE_2D);
	Image mollu("../../resource/bruaka_64.png", GL_TEXTURE_2D);

	float square[] =
	{
		0.5, 0.5, 0.0, 1.0, 1.0,
		-0.5, 0.5, 0.0, 0.0, 1.0,
		-0.5, -0.5, 0.0, 0.0, 0.0,
		0.5, -0.5, 0.0, 1.0, 0.0
	};
	unsigned int square_idx[] =
	{
		0, 1, 2,
		0, 2, 3
	};
	std::vector<Vertex> vert(4);
	std::vector<unsigned int> idx(6);
	std::vector<Texture> tex(2);

	// set vert
	int stride = 5;
	for(int i=0; i<vert.size(); i++)
	{
		vert[i].position = glm::vec3(square[stride*i], square[stride*i+1], square[stride*i+2]);
		vert[i].texCoord = glm::vec2(square[stride*i+3], square[stride*i+4]);
	}

	// set idx
	for(int i=0; i<idx.size(); i++) { idx[i] = square_idx[i]; }

	// set tex
	tex[0].textureID = hifumi.getImageID();
	tex[0].type = Texture::TYPE::DIFFUSE;
	tex[1].textureID = mollu.getImageID();
	tex[1].type = Texture::TYPE::DIFFUSE;

	Mesh myMesh(vert, idx, tex);

	//render loop
	while (!glfwWindowShouldClose(win))
	{
		//input

		//render background
		glClearColor(0.25f, 0.25f, 0.25, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//render objects
		basicSP.use();
		myMesh.draw(basicSP);

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

/*
GLuint VAO, VBO[2], EBO;

glGenVertexArrays(1, &VAO);
glGenBuffers(2, VBO);
glGenBuffers(1, &EBO);

glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
glBufferData(GL_ARRAY_BUFFER, sizeof(square_color), square_color, GL_STATIC_DRAW);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
glEnableVertexAttribArray(1);

glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_idx), square_idx, GL_STATIC_DRAW);
*/