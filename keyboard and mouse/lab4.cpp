#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


const unsigned int window_width = 1000;
const unsigned int window_height = 800;

float r = 0.5f;
int N = 10;
unsigned VAO, VBO, EBO;
unsigned shaderProgram;

std::vector<float> vertices;
std::vector<unsigned> indices;

void createCircle(float r, float N) {
    float phi = 360.f / N;
    int triangleCount = N - 2;

    vertices.clear();
    indices.clear();

    for (int i = 0; i < N; i++) {
        float currentPhi = phi * i;
        float x = r * cos(currentPhi * (3.14f / 180.f));
        float y = r * sin(currentPhi * (3.14f / 180.f));
        float z = 0.0f;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
    }
    for (int i = 0; i < triangleCount; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }
}

// scroll callback - powiekszanie kola scrollem
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        N++;
    }
    else {
        if (N > 8) {
            N--;
        }
    }

    createCircle(r, N);

    // aktualizacja buforow wierzcholkow
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);

}

// polling - zmiana koloru kola (1, 2, 3)
void processInputKeyboard(GLFWwindow* window, unsigned shaderProgram) {
    // change kolor with 1, 2, 3 keys
    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        glUseProgram(shaderProgram);
        glUniform3f(uColorLocation, 1.0, 1.0, 0.0);
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        glUseProgram(shaderProgram);
        glUniform3f(uColorLocation, 0.0, 1.0, 0.0);
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        glUseProgram(shaderProgram);
        glUniform3f(uColorLocation, 0.0, 1.0, 1.0);
    }
}

const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 fragmentColor;\n"
"uniform vec3 uColor = vec3(0.5, 0.5, 0.5);"
"void main()\n"
"{\n"
" fragmentColor = vec4(uColor, 1.0);\n"
"}\0";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window setup
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Lab 4", NULL, NULL);
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

    // aktywowanie funkcji
    glfwSetScrollCallback(window, scrollCallback);

    // cirle setup
    std::cout << "r = " << r;
    createCircle(r, N);

    /* SHADERS */
    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);


    /* BUFFERS */
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);


    // uColor setup
    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");
    glUseProgram(shaderProgram);
    glUniform3f(uColorLocation, 1.0f, 0.0f, 0.0f);


	//petla zdarzen
    while (!glfwWindowShouldClose(window)) {

        // Obs³uga zdarzeñ
        glfwPollEvents();

        // Wyczyszczenie bufora koloru
        glClearColor(0.7f, 0.0f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderowanie ko³a
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        processInputKeyboard(window, shaderProgram);
        glfwSwapBuffers(window);
    }

    // Zwolnienie zasobów
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Zakoñczenie dzia³ania GLFW
    glfwTerminate();
    return 0;
}