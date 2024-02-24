#pragma once

#include <vulkan/vulkan.h>

#include "Log.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    switch (messageSeverity)
    {
    case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        CORE_LOG_INFO("Validation Layer: {0}", pCallbackData->pMessage);
        break;

    case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        CORE_LOG_ERROR("Validation Layer: {0}", pCallbackData->pMessage);
        break;

    default:
        CORE_LOG_INFO("Validation Layer: {0}", pCallbackData->pMessage);
        break;
    };

    return VK_FALSE;
};
