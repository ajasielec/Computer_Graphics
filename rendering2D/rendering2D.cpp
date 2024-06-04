#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

// shaders
const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec2 aPos;\n"

"uniform mat4 model;\n"
"uniform mat4 projection;\n"

"void main(){\n"
"gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"

"uniform vec3 color;\n"

"void main(){\n"
"    FragColor = vec4(color, 1.0);\n"
"}\0";


// Rozmiar okna
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const float BALL_SPEED = 10;

// Parametry ko³a
const float RADIUS = 25.0f;

// Zmienne globalne
glm::vec2 circlePos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
glm::vec2 circleVelocity(0.0f, 0.0f);
bool isMoving = false;

GLuint shaderProgram;
GLuint VAO, VBO, EBO;

std::vector<float> vertices;
std::vector<unsigned int> indices;

// Funkcje do kompilacji shaderów
std::string readShaderSource(const char* filePath) {
    std::string code;
    std::ifstream shaderFile(filePath, std::ios::in);

    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open " << filePath << std::endl;
        return "";
    }

    std::string line;
    while (getline(shaderFile, line)) {
        code += line + "\n";
    }

    shaderFile.close();
    return code;
}

GLuint compileShader(const char* filePath, GLenum shaderType) {
    std::string shaderSource = readShaderSource(filePath);
    const char* shaderCode = shaderSource.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

void createCircle() {
    int numSegments = 100;
    vertices.clear();
    indices.clear();

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= numSegments; ++i) {
        float angle = i * 2.0f * 3.14159f / numSegments;
        float dx = cos(angle);
        float dy = sin(angle);
        vertices.push_back(dx);
        vertices.push_back(dy);

        if (i > 0) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }
    indices.back() = 1; // Closing the circle
}

void setupBuffers() {
    createCircle();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateCircle(float deltaTime, float speed) {
    if (!isMoving) return;

    circlePos += circleVelocity * deltaTime * speed;

    // Detekcja kolizji z krawêdziami okna
    bool collision = false;
    // lewa krawedz
    if (circlePos.x - RADIUS < 0) {
        circlePos.x = RADIUS;
        circleVelocity.x *= -1;
        collision = true;
    }
    // prawa krawedz
    else if (circlePos.x + RADIUS > WINDOW_WIDTH) {
        circlePos.x = WINDOW_WIDTH - RADIUS;
        circleVelocity.x *= -1;
        collision = true;
    }

    // gorna krawedz
    if (circlePos.y - RADIUS < 0) {
        circlePos.y = RADIUS;
        circleVelocity.y *= -1;
        collision = true;
    }
    // dolna krawedz
    else if (circlePos.y + RADIUS > WINDOW_HEIGHT) {
        circlePos.y = WINDOW_HEIGHT - RADIUS;
        circleVelocity.y *= -1;
        collision = true;
    }

    if (collision) {
        // Zmiana koloru na losowy
        glUseProgram(shaderProgram);
        glUniform3f(glGetUniformLocation(shaderProgram, "color"),
            glm::linearRand(0.0f, 1.0f),
            glm::linearRand(0.0f, 1.0f),
            glm::linearRand(0.0f, 1.0f));
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (!isMoving) {
            // Losowanie pocz¹tkowego kierunku ruchu
            circleVelocity = glm::ballRand(100.0f); // Prêdkoœæ losowa
            isMoving = true;
        }
    }
}


int main() {
    // Inicjalizacja GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "rendering 2D", NULL, NULL);
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

    glViewport(0, 0, (GLuint)WINDOW_WIDTH, (GLuint)WINDOW_HEIGHT);


    // Ustawienie callbacku klawiszy
    glfwSetKeyCallback(window, keyCallback);

    // Ustawienie koloru t³a
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Kompilacja shaderów
    GLuint vertexShader = compileShader("vertex_shader.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader("fragment_shader.glsl", GL_FRAGMENT_SHADER);

    // Linkowanie shaderów
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 1.0f, 1.0f);

    setupBuffers();

    // Ustawienie macierzy projekcji
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

    // Wektor przechowuj¹cy poprzedni czas
    float lastFrame = 0.0f;

    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        // Obliczenie deltaTime
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Aktualizacja pozycji ko³a
        updateCircle(deltaTime, BALL_SPEED);

        // Czyszczenie ekranu
        glClear(GL_COLOR_BUFFER_BIT);

        // Rysowanie ko³a
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(circlePos, 0.0f));
        model = glm::scale(model, glm::vec3(RADIUS, RADIUS, 1.0f));

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Wymiana buforów
        glfwSwapBuffers(window);

        // Przetwarzanie zdarzeñ
        glfwPollEvents();
    }

    // Czyszczenie zasobów
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
