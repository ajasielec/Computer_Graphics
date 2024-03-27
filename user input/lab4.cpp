#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>
#include <string.h>
#include <vector>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const unsigned int window_width = 1000;
const unsigned int window_height = 800;

float r = 0.5f;
int N = 10;
GLuint VAO, VBO, EBO;
GLuint shaderProgram;

const GLchar* image_path;

std::vector<float> vertices;
std::vector<unsigned> indices;

const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec2 texture;\n"
"out vec2 vertexTexture;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(position, 1.0);\n"
"   vertexTexture = texture;\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"in vec2 vertexTexture;\n"
"out vec4 fragmentColor;\n"
"uniform sampler2D uTexture;\n"
"uniform vec3 uColor = vec3(0.5, 0.5, 0.5);"
"void main()\n"
"{\n"
"   fragmentColor = texture(uTexture, vertexTexture.xy) * vec4(uColor, 1.0);\n"
"}\0";

void createCircle(float r, float N) {
    float phi = 360.f / N;
    int triangleCount = N - 2;
    float toRadians = 3.1415f / 180.f;

    vertices.clear();
    indices.clear();

    for (int i = 0; i < N; i++) {
        float currentPhi = phi * i;
        float x = r * cos(currentPhi * toRadians);
        float y = r * sin(currentPhi * toRadians);
        float z = 0.0f;
        float textureX = (cos(currentPhi * toRadians) + 1.0f) / 2.0f;   // mapping
        float textureY = (sin(currentPhi * toRadians) + 1.0f) / 2.0f;
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(textureX);
        vertices.push_back(textureY);
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

bool pressed = 0;
// polling - zmiana koloru kola (1, 2, 3)
void processInputKeyboard(GLFWwindow* window) {
    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        glUniform3f(uColorLocation, 1.0, 0.0, 0.0);
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        glUniform3f(uColorLocation, 0.0, 1.0, 0.0);
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        glUniform3f(uColorLocation, 0.0, 0.0, 1.0);
    }

    static double lastKeyPressTime = 0.0;
    static const double debounceDelay = 0.25;

    double currentTime = glfwGetTime();
   
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (currentTime - lastKeyPressTime) > debounceDelay) {
        if (!pressed) {
            image_path = "wall.jpg";
            pressed = 1;
        }
        else {
            image_path = "car.jpg";
            pressed = 0;
        }
        lastKeyPressTime = currentTime;

        GLint width, height, nrChannels;
         stbi_set_flip_vertically_on_load(true);
        GLubyte* data = stbi_load(image_path, &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
}


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

    // TEXTURE SETUP //
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLint width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    image_path = "car.jpg";
    GLubyte* data = stbi_load(image_path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // aktywowanie funkcji
    glfwSetScrollCallback(window, scrollCallback);

    // cirle setup
    std::cout << "r = " << r;
    createCircle(r, N);

    /* SHADERS */
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // uTexture setup
    //GLint uTextureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    //glUseProgram(shaderProgram);
    //glUniform1i(uTextureLocation, 0); // Bind texture unit 0


    // uColor setup
    GLint uColorLocation = glGetUniformLocation(shaderProgram, "uColor");
    glUseProgram(shaderProgram);
    glUniform3f(uColorLocation, 1.0f, 1.0f, 1.0f);



	//petla zdarzen
    while (!glfwWindowShouldClose(window)) {

        // Obs³uga zdarzeñ
        glfwPollEvents();

        // Wyczyszczenie bufora koloru
        glClearColor(0.7f, 0.0f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderowanie ko³a
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        processInputKeyboard(window);
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