#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>


const GLchar* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"layout(location = 1) in vec3 normal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 vertexNormal;\n"
"out vec3 fragmentPosition;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"    fragmentPosition = vec3(model * vec4(position, 1.0));\n"
"    vertexNormal = mat3(transpose(inverse(model))) * normal;\n"
"}\0";

const GLchar* fragmentShaderSource =
"#version 330 core\n"
"in vec3 vertexNormal;\n"
"in vec3 fragmentPosition;\n"
"uniform vec3 lightPosition;\n"
"uniform vec3 viewPosition;\n"

"uniform float ambientStrength = 0.15;\n"
"uniform bool diffuseFlag = true;\n"
"uniform float specularStrength = 0.5;\n"

"out vec4 fragmentColor;\n"

"void main()\n"
"{\n"
"    vec3 ambientColor = ambientStrength * vec3(1.0f, 1.0f, 1.0f);\n"

"    vec3 normalNormalized = normalize(vertexNormal);\n"
"    vec3 lightDirection = normalize(lightPosition - fragmentPosition);\n"
"    float diffuse = max(dot(normalNormalized, lightDirection), 0.0);\n"
"    vec3 diffuseColor = diffuse * vec3(1.0f, 1.0f, 1.0f);\n"
"    if(!diffuseFlag){\n"
"    diffuseColor = vec3(0.0f, 0.0f, 0.0f);}\n"

"    vec3 viewDirection = normalize(viewPosition - fragmentPosition);\n"
"    vec3 reflectDirection = reflect(-lightDirection, normalNormalized);\n"
"    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), 64);\n"
"   vec3 specularColor = specularStrength * specular * vec3(1.0f, 1.0f, 1.0f);\n"
"   fragmentColor = vec4(vec3(0.0f, 1.0f, 0.0f) * (ambientColor + diffuseColor + specularColor), 1.0);\n"
"}\0";

const GLchar* vertexLightSource =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"}\0";

const GLchar* fragmentLightSource =
"#version 330 core\n"
"out vec4 fragmentColor;\n"
"void main()\n"
"{\n"
"    fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"}\0";


const unsigned int window_width = 1600;
const unsigned int window_height = 800;

float pitch = 0.0f;
float yaw = -90.0f;

float previousX = static_cast<float>(1000) / 2.0f;
float previousY = static_cast<float>(800) / 2.0f;
float xdifference = 0.0f;
float ydifference = 0.0f;
bool first_window_enter = true;
const float sensitivity = 0.075f;

float deltaTime = 0.0f;
float previousTime = 0.0f;
float lastClick = 0.0f;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool ambient = true, diffuse = true, specular = true;

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals
) {
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Impossible to open the file!" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string lineHeader;
        ss >> lineHeader;

        if (lineHeader == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (lineHeader == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (lineHeader == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (lineHeader == "f") {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> vertexIndex[i] >> slash >> uvIndex[i] >> slash >> normalIndex[i];
            }
            if (ss.fail()) {
                std::cerr << "File can't be read by our simple parser: Try exporting with other options" << std::endl;
                return false;
            }
            for (int i = 0; i < 3; i++) {
                vertexIndices.push_back(vertexIndex[i]);
                uvIndices.push_back(uvIndex[i]);
                normalIndices.push_back(normalIndex[i]);
            }
        }
    }

    // Process the indices and push the corresponding vertices, uvs, and normals to the output vectors
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        out_vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < uvIndices.size(); i++) {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        out_uvs.push_back(uv);
    }

    for (unsigned int i = 0; i < normalIndices.size(); i++) {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[normalIndex - 1];
        out_normals.push_back(normal);
    }

    return true;
}

// poruszanie kamery wsadem
void processInputKeyboard(GLFWwindow* window) {
    const float cameraSpeed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    lastClick += deltaTime;
}

// poruszanie kamery myszka
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static float previousX = static_cast<float>(window_width) / 2.0f;
    static float previousY = static_cast<float>(window_height) / 2.0f;

    float xdifference = xpos - previousX;
    float ydifference = previousY - ypos;

    previousX = xpos;
    previousY = ypos;

    const float sensitivity = 0.075f;
    xdifference *= sensitivity;
    ydifference *= sensitivity;

    pitch += ydifference;
    yaw += xdifference;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 cameraFront_new;
    cameraFront_new.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront_new.y = sin(glm::radians(pitch));
    cameraFront_new.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront_new);
}

void AmbientLight(GLint AmbientFlagLoc) {
    if (ambient)
        glUniform1f(AmbientFlagLoc, 0.0f);
    else
        glUniform1f(AmbientFlagLoc, 0.15f);
    ambient = !ambient;
}


void DiffuseLight(GLint DiffuseFlagLoc) {
    if (diffuse)
        glUniform1f(DiffuseFlagLoc, false);
    else
        glUniform1f(DiffuseFlagLoc, true);
    diffuse = !diffuse;
}

void SpecularLight(GLint specularStrengthLoc) {
    if (specular)
        glUniform1f(specularStrengthLoc, 0.0f);
    else
        glUniform1f(specularStrengthLoc, 0.5f);
    specular = !specular;
}


int main()
{
    // inicjalizacja GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Tworzenie okna
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "grafika komputerowa", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // shadery
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint vertexLightShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexLightShader, 1, &vertexLightSource, NULL);
    glCompileShader(vertexLightShader);

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

    GLuint fragmentShaderLight = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderLight, 1, &fragmentLightSource, NULL);
    glCompileShader(fragmentShaderLight);

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

    GLuint shaderProgramLight = glCreateProgram();
    glAttachShader(shaderProgramLight, vertexLightShader);
    glAttachShader(shaderProgramLight, fragmentShaderLight);
    glLinkProgram(shaderProgramLight);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
        std::cout << "Error (Shader program): " << error_message << std::endl;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDetachShader(shaderProgramLight, vertexLightShader);
    glDetachShader(shaderProgramLight, fragmentShaderLight);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glDeleteShader(vertexLightShader);
    glDeleteShader(fragmentShaderLight);


    // cube geometry
    GLfloat vertices[] = {
        // vertices coords      // normals 
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f
    };

    // VAO, VBO
    GLuint VAO[2];
    GLuint VBO[3];

    glGenVertexArrays(2, VAO);
    glGenBuffers(3, VBO);

    std::vector< glm::vec3 > verticesL;
    std::vector< glm::vec2 > uvsL;
    std::vector< glm::vec3 > normalsL;
    bool res = loadOBJ("model.obj", verticesL, uvsL, normalsL);

    // model obj
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, verticesL.size() * sizeof(glm::vec3), &verticesL[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normalsL.size() * sizeof(glm::vec3), &normalsL[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // swiecacy szescian
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


    glEnable(GL_DEPTH_TEST);

    // macierze modelu
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 model2 = glm::mat4(1.0f);
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint modelLoc2 = glGetUniformLocation(shaderProgramLight, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // macierze widoku   
    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    glm::mat4 view2 = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint viewLoc2 = glGetUniformLocation(shaderProgramLight, "view");


    // scroll callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    // macierz projekcji
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
    glm::mat4 projection2 = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint projectionLoc2 = glGetUniformLocation(shaderProgramLight, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // view and perspective configuration
    glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);
    glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);

    float pos = 0;
    bool flag = true;
    float speed = 1.0f;
    GLuint ambientStrengthgLoc = glGetUniformLocation(shaderProgram, "ambientStrength");
    GLuint diffuseFlagLoc = glGetUniformLocation(shaderProgram, "diffuseFlag");
    GLuint specularStrengthLoc = glGetUniformLocation(shaderProgram, "specularStrength");

    GLuint lightPositionLoc = glGetUniformLocation(shaderProgram, "lightPosition");
    GLuint cameraPositionLoc = glGetUniformLocation(shaderProgram, "viewPosition");


    // MAIN LOOP
    while (!glfwWindowShouldClose(window))
    {
        // delta time
        float currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        
        // renderowanie
        glClearColor(0.2f, 0.1f, 0.141f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInputKeyboard(window);
        glUseProgram(shaderProgram);

        // ruch swiecacego szescianu
        float change = deltaTime * speed;
        if (flag) {
            pos += change;
        }
        else {
            pos -= change;
        }
        if (pos > 1.5f || pos < -1.5f) {
            flag = !flag;
        }


        lightPosition = glm::vec3(0.5f * pos, 0.8f, 1.0f);
        glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);

        // aktualizacja widoku kamery
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // macierz modelu
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


        // zmiana oswietlenia
        if (lastClick > 0.5f) {
            // 1 - ambient light
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                lastClick = 0.0f;
                AmbientLight(ambientStrengthgLoc);
            }
            // 2 - diffuse light
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                lastClick = 0.0f;
                DiffuseLight(diffuseFlagLoc);
            }
            // 3 - specular light
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                lastClick = 0.0f;
                SpecularLight(specularStrengthLoc);
            }
        }

        // camera update
        glUniform3f(cameraPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        // macierz projekcji
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouseCallback);

        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, verticesL.size() * 3);
        glBindVertexArray(0);

        // swiatlo
        glUseProgram(shaderProgramLight);

        // miaczerze modelu, widoku, projekcji
        model2 = glm::mat4(1.0f);
        model2 = glm::translate(model, glm::vec3(10.0f * pos, 100.9f, 1.0f));
        model2 = glm::scale(model2, glm::vec3(12.0f, 12.0f, 12.0f));
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
        
        view2 = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view2));

        projection2 = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, glm::value_ptr(projection2));

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(3, VBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgramLight);

    glfwTerminate();
    return 0;
}