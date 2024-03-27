#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// VAO VBO EBO
unsigned int VBO[2], VAO[2], EBO[2];
const GLchar* image_path;
GLint width, height, nrChannels;
GLuint texture;
GLuint shaderProgram;

const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec2 textureCoord;\n"
"out vec3 vertexColor;\n"
"out vec2 texCoord;\n"
"void main(){\n"
"gl_Position = vec4(position, 1.0);\n"
"vertexColor = color;\n"
"texCoord = textureCoord;\n"
"}\n\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"in vec3 vertexColor;\n"
"in vec2 texCoord;\n"
"out vec4 fragmentColor;\n"
"uniform sampler2D uTexture;\n"
"uniform vec3 customColor;\n"
"void main(){\n"
"vec4 texColor = texture(uTexture, texCoord);\n"
"fragmentColor = vec4(mix(texColor.rgb, customColor, 0.5), texColor.a);\n"
"}\n\0";

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    static float colorMix = 0.5f;
    colorMix += yoffset * 0.1f; // Adjust scrolling sensitivity here
    if (colorMix > 1.0f)
        colorMix = 1.0f;
    else if (colorMix < 0.0f)
        colorMix = 0.0f;
    GLint customColorLocation = glGetUniformLocation(shaderProgram, "customColor");
    glUseProgram(shaderProgram);
    glUniform3f(customColorLocation, colorMix, 0.0f, 0.0f);
}

bool draw = 0;
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        image_path = "wall.jpg";
        glBindVertexArray(VAO[0]);
        draw = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        glBindVertexArray(VAO[1]);
        image_path = "cat.jpg";
        draw = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        draw = 1;
    }

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const unsigned int window_width = 1000;
    const unsigned int window_height = 800;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGL Window", NULL, NULL);
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

    glViewport(0, 0, window_width, window_height);

    // TEXTURE SETUP //
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   // GLint width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    image_path = "cat.jpg";
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

    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

    float verticesTriangle[] = {
        // positions          // colors           // texture coords
        0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.5f, 1.0f, // top 
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // left 
        0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 0.0f  // right 
    };

    unsigned indicesTriangle[] = {
     0, 1, 2		// triangle 1
    };

    float verticesSquare[] = {
        // positions          // colors           // texture coords
        0.8f,  0.8f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.8f, -0.8f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.8f,  0.8f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    unsigned indicesSquare[] = {
        0, 1, 3,		// triangle 1
        1, 2, 3			// triangle 2
    };

    // SQUARE //
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);		//coordinates attribute
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesSquare), indicesSquare, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    // TRIANGLE //
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTriangle), verticesTriangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTriangle), indicesTriangle, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    // SHADERS //
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set uniforms
   // glUseProgram(shaderProgram);
   // glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);

        if (draw) {
            image_path = "wall.jpg";
            GLubyte* data = stbi_load(image_path, &width, &height, &nrChannels, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            glBindVertexArray(VAO[0]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            image_path = "cat.jpg";
            data = stbi_load(image_path, &width, &height, &nrChannels, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
          
            glBindVertexArray(VAO[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Check and call events and swap the buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Deallocate resources
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(2, EBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();
    return 0;
}