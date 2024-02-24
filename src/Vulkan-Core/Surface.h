#pragma once


#include <vulkan/vulkan.h>

#include "../Common.h"
#include "Instance.h"


namespace VulkanCore
{
    class Surface
    {
    public:
        Surface() = default;
        ~Surface() = default;

        void Create(const Instance &instance, void *window);
        void Destroy();

        VkSurfaceKHR Get() { return m_Surface; };
        void *GetWindow() { return m_WindowHandle; };

    private:
        VkSurfaceKHR m_Surface;
        Instance m_Instance;
        void *m_WindowHandle;
    };

};