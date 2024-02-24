#pragma once

#include <GLFW/glfw3.h>

struct AppInstanceData
{
    uint32_t width;
    uint32_t height;
    const char * title;
    void *window = nullptr;
};

class Layer
{
public:
    virtual void OnInit(const AppInstanceData &appInstanceData) = 0;
    virtual void OnPrepareFrame() = 0;
    virtual void OnRenderFrame() = 0;
    virtual void OnCleanup() = 0;
    virtual void OnResize(int width, int height) = 0;
};

struct ApplicationConfig
{
    uint32_t width;
    uint32_t height;
    const char *title;
    Layer *layer = nullptr;
};

class Application
{
public:
    Application(const ApplicationConfig &config);
    ~Application();

    void Run();
    GLFWwindow *GetWindow() { return m_Window; };

private:
    Layer *m_Layer = nullptr;
    GLFWwindow *m_Window = nullptr;
    AppInstanceData m_AppData;
};

