#pragma once

#include "../Common.h"

#include <vulkan/vulkan.h>

namespace VulkanCore
{

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct DeviceSupport
    {
        bool swapChain;
        bool isDiscrete;
        const std::vector<const char *> queueFamilies;
        const std::vector<const char *> extensions;
        bool geometryShader;
    };

    struct InstanceConfig
    {
        bool enableValidation = false;
        PFN_vkDebugUtilsMessengerCallbackEXT debugCallback = nullptr;
        const char *appName;
        const char *engineName;
        uint32_t apiVersion;
    };

    struct DeviceConfig
    {
        std::vector<const char *> requiredExtensions;
        bool requireGraphicsQueue;
        bool requirePresentQueue;
        bool isDiscrete;
    };

    struct SwapChainConfig
    {
        VkFormat format;
        VkColorSpaceKHR colorSpace;
        VkPresentModeKHR presentMode;
        uint32_t width;
        uint32_t height;
    };

};