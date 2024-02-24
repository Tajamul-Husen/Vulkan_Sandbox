#pragma once

#include <vulkan/vulkan.h>

#include "../Common.h"
#include "Types.h"
#include "Instance.h"
#include "Surface.h"

namespace VulkanCore
{
    class Device
    {
    public:
        Device() = default;
        ~Device() = default;

	    void Create(const DeviceConfig &config, const Instance &instance, const Surface &surface);
        void Destroy();

        VkPhysicalDevice GetPhysical() { return m_PhysicalDevice; };
        VkDevice Get() { return m_Device; };
        QueueFamilyIndices GetQueueFamilies() { return m_QueueFamilies; };
        VkQueue GetGraphicsQueue() { return m_GraphicsQueue; };
        VkQueue GetPresentQueue() { return m_PresentQueue; };
        const std::string& GetDeviceName() { return m_SelectedDeviceName; };

    private:
        void PickPhysical();
        bool IsSuitable(VkPhysicalDevice device);
        VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice device);
        VkPhysicalDeviceFeatures GetDeviceFeatures(VkPhysicalDevice device);
        std::vector<VkQueueFamilyProperties> GetQueueFamilies(VkPhysicalDevice device);
        std::optional<uint32_t> GetPresentQueueIndex(const std::vector<VkQueueFamilyProperties> &queueFamilies, VkPhysicalDevice device, VkSurfaceKHR surface);
        std::optional<uint32_t> GetQueueIndex(const std::vector<VkQueueFamilyProperties> &queueFamilies, VkQueueFlagBits queueFlag);
        std::vector<VkExtensionProperties> GetRequiredExtensions(VkPhysicalDevice device, const std::vector<const char*> &requiredExtensions);

    private:
        DeviceConfig m_DeviceConfig;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        QueueFamilyIndices m_QueueFamilies;
        std::vector<const char *> m_Extensions;
        Instance m_Instance;
        Surface m_Surface;
        std::string m_SelectedDeviceName;
    };

};