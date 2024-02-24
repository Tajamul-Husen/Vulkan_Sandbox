#pragma once

#include "../Common.h"

#include <vulkan/vulkan.h>
#include "Types.h"

namespace VulkanCore
{
    namespace Utils
    {

        inline SwapChainSupportDetails GetSwapChainDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
        {
            SwapChainSupportDetails swapChainDetails;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.capabilities);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

            if (formatCount != 0)
            {
                swapChainDetails.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
            };

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

            if (presentModeCount != 0)
            {
                swapChainDetails.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapChainDetails.presentModes.data());
            };

            return swapChainDetails;
        };

        inline std::optional<uint32_t> FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                };
            };

            return {};
        };
    };
};