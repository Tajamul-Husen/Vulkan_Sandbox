#pragma once

#include <vulkan/vulkan.h>

#include "../Common.h"
#include "Types.h"

namespace VulkanCore
{

    class Instance
    {
    public:
        Instance() = default;
        ~Instance() = default;

        void Create(const InstanceConfig &config);
        void Destroy();

        VkInstance Get() { return m_Instance; };

    private:
        bool CheckValidationLayerSupport();
        std::vector<const char *> GetRequiredExtensions();

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

    private:
        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        InstanceConfig m_InstanceConfigData;
        bool m_ValidationLayerEnabled = false;
        std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    };

};
