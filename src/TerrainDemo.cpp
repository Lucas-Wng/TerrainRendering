//
// Created by Lucas Wang on 2024-06-14.
//

#include "TerrainDemo.h"

static void ResizeCallback(GLFWwindow*, int, int);
static void MouseMoveCallback(GLFWwindow*, double, double);
static void KeyPressedCallback(GLFWwindow*, int, int, int, int);

const int SCR_WIDTH = 1920;
const int SCR_HEIGHT = 1080;

TerrainDemo::TerrainDemo() {
    Init();
    Run();
}

TerrainDemo::~TerrainDemo() {

}

void TerrainDemo::Init() {
    CreateWindow();
    SetCallbacks();
    CreateShaders();
    CreateCamera();
    InitTerrain();
}

void TerrainDemo::CreateWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(0);
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(0);
    }

    glEnable(GL_DEPTH_TEST);
}

void TerrainDemo::InitTerrain() {
    m_terrain = new Terrain();
    m_terrain->Generate();
}

void TerrainDemo::SetCallbacks() {
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window,ResizeCallback);
    glfwSetCursorPosCallback(m_window,MouseMoveCallback);
    glfwSetKeyCallback(m_window, KeyPressedCallback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void TerrainDemo::CreateShaders() {
    m_shader = new Shader("resources/shaders/terrain.vs", "resources/shaders/terrain.fs");
}

void TerrainDemo::CreateCamera() {
    m_camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

void TerrainDemo::Run() {
    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;
        ProcessInput();
        Render();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void TerrainDemo::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = m_camera->GetViewMatrix();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    m_shader->setMat4("model", model);

    m_terrain->Render();
}

void TerrainDemo::ProcessInput() { //make this only have the normal inputs
    for (int key = 0; key < 1024; ++key) {
        if (m_keys[key]) {
            OnKeyPressed(m_window, key, 0, GLFW_PRESS, 0);
        }
    }
}

static void ResizeCallback(GLFWwindow *w, int width, int height) {
    auto app = (TerrainDemo*)glfwGetWindowUserPointer(w);
    app->OnResize(w, width, height);
}

static void MouseMoveCallback(GLFWwindow *w, double x, double y) {
    auto app = (TerrainDemo*)glfwGetWindowUserPointer(w);
    app->OnMouseMove(w, x, y);
}

static void KeyPressedCallback(GLFWwindow *w, int key, int scancode,
                               int action, int mods) {
    auto app = (TerrainDemo*)glfwGetWindowUserPointer(w);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        app->m_keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        app->m_keys[key] = false;
    }
}

void TerrainDemo::OnResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void TerrainDemo::OnMouseMove(GLFWwindow* window, double x, double y) {
    float xpos = static_cast<float>(x);
    float ypos = static_cast<float>(y);

    if (firstMouse) {
        m_lastX = xpos;
        m_lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top

    m_lastX = xpos;
    m_lastY = ypos;

    m_camera->ProcessMouseMovement(xoffset, yoffset);
}

void TerrainDemo::OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key == GLFW_KEY_W) {
        m_camera->ProcessKeyboard(FORWARD, m_deltaTime);
    }
    if (key == GLFW_KEY_S) {
        m_camera->ProcessKeyboard(BACKWARD, m_deltaTime);
    }
    if (key == GLFW_KEY_A) {
        m_camera->ProcessKeyboard(LEFT, m_deltaTime);
    }
    if (key == GLFW_KEY_D) {
        m_camera->ProcessKeyboard(RIGHT, m_deltaTime);
    }
    if (key == GLFW_KEY_SPACE) {
        m_camera->ProcessKeyboard(UP, m_deltaTime);
    }
    if (key == GLFW_KEY_LEFT_SHIFT) {
        m_camera->ProcessKeyboard(DOWN, m_deltaTime);
    }
}