#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Hypercube.h"
#include <iostream>
#include <vector>


//Vertex Shader passes 3D positions to OpenGL
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor; //Color input
    out vec3 vertexColor;                 //Pass to fragment shader
    void main() {
        gl_Position = vec4(aPos * 0.5, 1.0);
        gl_PointSize = 6.0;
        vertexColor = aColor;
    }
)";

//Fragment Shader colors in red and blue
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor; //Receive from vertex shader
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor, 1.0); //Use the calculated color
    }
)";

//Helper function to compile shaders
unsigned int CompileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Error:\n" << infoLog << "\n";
    }
    return id;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        Hypercube* hc = static_cast<Hypercube*>(glfwGetWindowUserPointer(window));
        if (hc) {
            if (key == GLFW_KEY_1) hc->toggleRotation(0);
            if (key == GLFW_KEY_2) hc->toggleRotation(1);
            if (key == GLFW_KEY_3) hc->toggleRotation(2);
            if (key == GLFW_KEY_4) hc->toggleRotation(3);
            if (key == GLFW_KEY_5) hc->toggleRotation(4);
            if (key == GLFW_KEY_6) hc->toggleRotation(5);
        }
    }
}

int main()
{
    //Initialize GLFW and GLAD
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Hypercube 4D Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    //Setup Hypercube
    Hypercube hypercube;
    glfwSetWindowUserPointer(window, &hypercube);
    glfwSetKeyCallback(window, key_callback);

    //Enable setting point size in the vertex shader
    glEnable(GL_PROGRAM_POINT_SIZE);

    //Build the Shader Program
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    //Setup Hypercube and Data
    //Flatten the edges from pairs into a single vec[int] for OpenGL
    auto edgesPairs = hypercube.getEdges();
    std::vector<unsigned int> edgeIndices;
    for (const auto& edge : edgesPairs) {
        edgeIndices.push_back(edge.first);
        edgeIndices.push_back(edge.second);
    }

    //Setup VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    //VBO allocate space for 16 Vec3
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Allocate space for Vertex3D (Position + Color)
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(Vertex3D), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndices.size() * sizeof(unsigned int), edgeIndices.data(), GL_STATIC_DRAW);

    //Tell OpenGL how to read the Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
    glEnableVertexAttribArray(0);

    //Tell OpenGL how to read the Colors (last 3 floats, offset by 12 bytes)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Render Loop
    float lastTime = glfwGetTime();
    std::string lastTitle = "";

    while (!glfwWindowShouldClose(window))
    {
        //Calculate Delta Time
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        //Update Window Title with active planes
        std::string currentPlanes = hypercube.getActivePlanesString();
        if (currentPlanes != lastTitle) {
            std::string fullTitle = "Hypercube 4D Viewer - " + currentPlanes;
            glfwSetWindowTitle(window, fullTitle.c_str());
            lastTitle = currentPlanes;
        }

        //Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //Update 4D Math
        hypercube.updateRotation(deltaTime);
        hypercube.projectTo3D(2.5f); //4D camera distance

        //Fetch new 3D vertices
        const auto& vertices = hypercube.getProjectedVertices();

        //Update the VBO with the new frame's vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 16 * sizeof(Vertex3D), vertices.data());

        //Render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); //Dark grey background
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        //Draw the lines
        glDrawElements(GL_LINES, edgeIndices.size(), GL_UNSIGNED_INT, 0);

        //Draw the vertices
        glDrawArrays(GL_POINTS, 0, 16);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}