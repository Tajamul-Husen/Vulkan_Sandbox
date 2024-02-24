#include "Application.h"
#include "Log.h"

Application::Application(const ApplicationConfig &config)
{
    CORE_LOG_INFO("Application Initialized.");

    m_Layer = config.layer;

    CORE_ASSERT(m_Layer != nullptr, "Render layer not passed!");

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(config.width, config.height, config.title, nullptr, nullptr);

    CORE_ASSERT(m_Window != nullptr, "Failed to create window!");

    glfwSetWindowUserPointer(m_Window, m_Layer);
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
                                   { 
        auto layer = reinterpret_cast<Layer *>(glfwGetWindowUserPointer(window));
        layer->OnResize(width, height); });

    m_AppData.width = config.width;
    m_AppData.height = config.height;
    m_AppData.title = config.title;
    m_AppData.window = m_Window;

    m_Layer->OnInit(m_AppData);
};

Application::~Application()
{
    delete m_Layer;
    glfwDestroyWindow(m_Window);
    glfwTerminate();

    CORE_LOG_INFO("Application Shutdown.");
};

void Application::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        m_Layer->OnPrepareFrame();
        m_Layer->OnRenderFrame();
        glfwPollEvents();
    };

    m_Layer->OnCleanup();
};