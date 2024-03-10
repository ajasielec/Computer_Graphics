#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// window setup
	const unsigned int window_width = 1000;
	const unsigned int window_height = 800;
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Rectangles :)", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);


	//VAOs, VBOs, EBO
	unsigned int VAOs[3], VBOs[3], EBOs[2];
	glGenVertexArrays(3, VAOs);
	glGenBuffers(3, VBOs);
	glGenBuffers(2, EBOs);

	unsigned indices[] = {
		0, 1, 3,		// triangle 1
		1, 2, 3			// triangle 2
	};

	// FIRST RECTANGLE SETUP
	float vertices[] = {
		// coordinates			//colors
		-0.8f, 0.6f, 0.0f,		0.0f, 1.0f, 0.0f,	// top left
		-0.8f, -0.6f, 0.0f,		0.0f, 1.0f, 0.0f,	// down left
		0.8f, -0.6f, 0.0f,		0.0f, 1.0f, 0.0f,	// down right
		0.8f, 0.6f, 0.0f,		0.0f, 1.0f, 0.0f,	// top right
	};

	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);		//coordinates attribute
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));		//color attribute
	glEnableVertexAttribArray(1);

	const GLchar* vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 color;\n"
		"out vec3 vertexColor;\n"
		"void main(){\n"
		"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"vertexColor = color;\n"
		"}\0";
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint status;
	GLchar error_message[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, error_message);
		std::cout << "Error (Vertex shader): " << error_message << std::endl;
	}

	const GLchar* fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 vertexColor;\n"
		"out vec4 fragmentColor;\n"
		"void main(){\n"
		"fragmentColor = vec4(vertexColor, 1.0);\n"
		"}\0";
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, error_message);
		std::cout << "Error (Fragment shader): " << error_message << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
		std::cout << "Error (Shader program): " << error_message << std::endl;
	}

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);


	// SECOND RECTANGLE SETUP
	float vertices2[] = {
		// coordinates
		-0.9f, -0.2f, 0.0f,		// top left
		-0.9f, -0.7f, 0.0f,		// down left
		-0.3f, -0.7f, 0.0f,		// down right
		-0.3f, -0.2f, 0.0f,		// top right
	};

	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);		//coordinates attribute
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));		//color attribute
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAOs[1]);		//aktywuje VAO
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);		//powiazanie ID z typem bufora
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);	//utworzenie bufora VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);		//atrybut wspolrzednych
	glEnableVertexAttribArray(0);	//aktywacja atrybutu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);		//powiazanie ID z typem bufora

	const GLchar* fragmentShaderSource2 =
		"#version 330 core\n"
		"in vec3 vertexColor;\n"
		"out vec4 fragmentColor;\n"
		"void main(){\n"
		"fragmentColor = vec4(0.9, 0.36, 0.74, 0.8);\n"
		"}\0";

	GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);		//tworzy shader
	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);		//przypisuje ID, przekazuje kod
	glCompileShader(fragmentShader2);	//kompilacja
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, error_message);
		std::cout << "Error (Fragment shader2): " << error_message << std::endl;
	}

	GLuint shaderProgram2 = glCreateProgram();	//utworzenie programu
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
		std::cout << "Error (Shader program 2): " << error_message << std::endl;
	}

	glDetachShader(shaderProgram2, vertexShader);
	glDetachShader(shaderProgram2, fragmentShader2);

	//TASK 2, third rectangle setup
	unsigned indices2[] = {
		3, 2, 1, 0,			// triangle 1
		1, 2, 3, 0			// triangle 2
	};

	float vertices3[] = {
		// coordinates			//colors
		-0.9f, 0.7f, 0.0f,		1.0f, 1.0f, 0.0f,	// top left
		-0.9f, -0.7f, 0.0f,		1.0f, 0.0f, 0.0f,	// down left
		0.9f, -0.7f, 0.0f,		0.0f, 1.0f, 1.0f,	// down right
		0.9f, 0.7f, 0.0f,		1.0f, 0.0f, 1.0f,	// top right
	};

	glBindVertexArray(VAOs[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);		//coordinates attribute
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));		//color attribute
	glEnableVertexAttribArray(1);

	GLuint shaderProgram3 = glCreateProgram();
	glAttachShader(shaderProgram3, vertexShader);
	glAttachShader(shaderProgram3, fragmentShader);
	glLinkProgram(shaderProgram3);
	glGetProgramiv(shaderProgram3, GL_LINK_STATUS, &status);
	if (!status) {
		glGetProgramInfoLog(shaderProgram3, 512, NULL, error_message);
		std::cout << "Error (Shader program 3): " << error_message << std::endl;
	}

	glDetachShader(shaderProgram3, vertexShader);
	glDetachShader(shaderProgram3, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragmentShader2);

	glBindVertexArray(0);
	glBindVertexArray(1);
	glBindVertexArray(2);

	char x = '0';
	bool show_deafult = 1;
	std::cout << "[1] - task 1\n[2] - task 2\n[Q] - return to default\n";

	while (!glfwWindowShouldClose(window)) {
		// window rendering
		glClearColor(0.2f, 0.1f, 0.141f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (show_deafult) {
			//rectangle rendering
			glUseProgram(shaderProgram);
			glBindVertexArray(VAOs[0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		std::cin >> x;

		switch (x)
		{
		case '1':
			// * Task 1 *
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(shaderProgram);
			glBindVertexArray(VAOs[0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glUseProgram(shaderProgram2);
			glBindVertexArray(VAOs[1]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			show_deafult = 0;
			break;

		case '2':
			// * Task 2 *
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(shaderProgram3);
			glBindVertexArray(VAOs[2]);
			glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

			show_deafult = 0;
			break;

		case 'Q': case 'q':
			show_deafult = 1;
			break;

		default:
			break;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(3, VAOs);
	glDeleteBuffers(3, VBOs);
	glDeleteBuffers(2, EBOs);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(shaderProgram2);
	glDeleteProgram(shaderProgram3);

	glfwTerminate();

	return 0;
}