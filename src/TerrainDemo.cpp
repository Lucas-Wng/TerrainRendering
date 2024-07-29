//
// Created by Lucas Wang on 2024-06-14.
//

#include "TerrainDemo.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <string>
#include "Skybox.h"

static void ResizeCallback(GLFWwindow*, int, int);
static void MouseMoveCallback(GLFWwindow*, double, double);
static void KeyPressedCallback(GLFWwindow*, int, int, int, int);

const int SCR_WIDTH = 1920;
const int SCR_HEIGHT = 1080;

const int world_width = 256;
const int world_depth = 256;

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
    InitSkybox();
    InitImGui();
    SetShaderUniforms();
}

void TerrainDemo::CreateWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TerrainRendering", NULL, NULL);
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
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void TerrainDemo::InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void TerrainDemo::InitTerrain() {
    m_terrain = new Terrain(world_width, world_depth, true);
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
    m_skyboxShader = new Shader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
}

void TerrainDemo::CreateCamera() {
    m_camera = new Camera(glm::vec3(2.0f, 4.0f, 2.0f));
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void TerrainDemo::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = m_camera->GetViewMatrix();
    m_shader->use();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    m_shader->setMat4("model", model);
    m_shader->setVec3("lightPos", glm::vec3(sin(glfwGetTime() * 0.1) * world_width * 2, 5.0f, cos(glfwGetTime() * 0.1   ) *
    world_width * 2));

    m_terrain->Render();

    m_skyboxShader->use();
    view = glm::mat4(glm::mat3(m_camera->GetViewMatrix())); // remove translation from the view matrix
    m_skyboxShader->setMat4("view", view);
    m_skyboxShader->setMat4("projection", projection);

    m_skybox->RenderSkybox();

    ImGui::Begin("ImGui Window");
    ImGui::Text("Hello ImGui");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TerrainDemo::ProcessInput() {
    if (m_keys[GLFW_KEY_W]) {
        m_camera->ProcessKeyboard(FORWARD, m_deltaTime);
    }
    if (m_keys[GLFW_KEY_S]) {
        m_camera->ProcessKeyboard(BACKWARD, m_deltaTime);
    }
    if (m_keys[GLFW_KEY_A]) {
        m_camera->ProcessKeyboard(LEFT, m_deltaTime);
    }
    if (m_keys[GLFW_KEY_D]) {
        m_camera->ProcessKeyboard(RIGHT, m_deltaTime);
    }
    if (m_keys[GLFW_KEY_SPACE]) {
        m_camera->ProcessKeyboard(UP, m_deltaTime);
    }
    if (m_keys[GLFW_KEY_LEFT_SHIFT]) {
        m_camera->ProcessKeyboard(DOWN, m_deltaTime);
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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(w, true);
    }
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

void TerrainDemo::SetShaderUniforms() {
    m_shader->use();
    m_shader->setVec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
    m_shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_shader->setVec3("objectColor", glm::vec3(0.2f, 0.2f, 0.2f));

    m_shader->setInt("diffuseMap", 0);
    m_shader->setInt("dispMap", 1);
    m_shader->setInt("normalMap", 2);
    m_shader->setInt("roughMap", 3);
}

void TerrainDemo::InitSkybox() {
    m_skybox = new Skybox();
    m_skybox->LoadCubeMap();
}