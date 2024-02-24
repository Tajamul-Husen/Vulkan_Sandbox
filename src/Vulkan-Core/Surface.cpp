#include "Surface.h"
#include "../Log.h"

#include <GLFW/glfw3.h>

namespace VulkanCore
{

    void Surface::Create(const Instance &instance, void *window)
    {
        m_Instance = instance;
        m_WindowHandle = window;
        GLFWwindow *win = static_cast<GLFWwindow *>(m_WindowHandle);

        VkResult result = glfwCreateWindowSurface(m_Instance.Get(), win, nullptr, &m_Surface);

        CORE_ASSERT(result == VK_SUCCESS, "Failed to create window surface!");
    };

    void Surface::Destroy()
    {
        vkDestroySurfaceKHR(m_Instance.Get(), m_Surface, nullptr);
    };

}