#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>


unsigned int VBO, VAO, EBO;
GLuint shaderProgram;
glm::mat4 model = glm::mat4(1.0f);

const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec3 vertexColor;\n"
"void main()\n"
"{\n"
"    gl_Position = model * vec4(position, 1.0);\n"
"    vertexColor = color;\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 fragmentColor;\n"
"uniform vec3 uColor = vec3(0.5, 0.5, 0.5);"
"void main()\n"
"{\n"
    " fragmentColor = vec4(uColor, 1.0); \n"
"}\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const unsigned int window_width = 1000;
    const unsigned int window_height = 800;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "grafika komputerowa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, error_message);
        std::cout << "Error (Fragment shader): " << error_message << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
        std::cout << "Error (Shader program): " << error_message << std::endl;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Definicja wierzcho³ków i indeksów dla trójk¹tów
    GLfloat vertices[] = {
        // Pozycje              // Kolory
        -0.25f, -0.25f, 0.0f,     1.0f, 0.0f, 0.0f, 
         0.0f,   0.25f, 0.0f,     1.0f, 0.0f, 0.0f, 
         0.25f, -0.25f, 0.0f,     1.0f, 0.0f, 0.0f  
    };

    GLuint indices[] = { 0, 1, 2 }; // pojedynczy trójk¹t

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);
   
    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Ustawianie macierzy modelu dla ka¿dego trójk¹ta
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.6f, 0.0f)); // lewy górny róg
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.6f, 0.0f));  // prawy górny róg
        glm::mat4 model3 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, -0.6f, 0.0f)); // lewy dolny róg
        glm::mat4 model4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, -0.6f, 0.0f));  // prawy dolny róg


        // Pobieranie czasu dla animacji
        float time = glfwGetTime();

        // Trójk¹t 1: ruch w linii poziomej
        float tx1 = 0.1f * cos(time); 
        model1 = glm::translate(model1, glm::vec3(tx1, 0.0f, 0.0f));

        // Trójk¹t 2: rotacja
        float angle2 = time * glm::radians(90.0f);
        model2 = glm::rotate(model2, angle2, glm::vec3(0.0f, 0.0f, 1.0f));

        // Trójk¹t 3: naprzemienne zwiêkszanie i zmniejszanie rozmiaru
        float scale3 = 0.5f + 0.2f * sin(time);
        model3 = glm::scale(model3, glm::vec3(scale3, scale3, scale3));

        // Trójk¹t 4: po³¹czenie powy¿szych ruchów
        float tx4 = 0.2f * cos(time);
        float ty4 = 0.1f * sin(time);
        float angle4 = time * glm::radians(90.0f);
        float scale4 = 0.5f + 0.2f * sin(time);

        model4 = glm::translate(model4, glm::vec3(tx4, ty4, 0.0f));
        model4 = glm::rotate(model4, angle4, glm::vec3(0.0f, 0.0f, 1.0f));
        model4 = glm::scale(model4, glm::vec3(scale4, scale4, scale4));
      
        glUseProgram(shaderProgram);

        // Trójk¹t 1
        glUniform3f(uColorLocation, 1.0f, 0.0f, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model1));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Trójk¹t 2
        glUniform3f(uColorLocation, 1.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Trójk¹t 3
        glUniform3f(uColorLocation, 0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model3));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Trójk¹t 4
        glUniform3f(uColorLocation, 1.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model4));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}