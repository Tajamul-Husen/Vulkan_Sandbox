#include "SwapChain.h"
#include "Utils.h"
#include "../Log.h"

#include <GLFW/glfw3.h>

namespace VulkanCore
{

    void SwapChain::Create(const SwapChainConfig &config, const Device &device, const Surface &surface)
    {
        m_Config = config;
        m_DeviceInst = device;
        m_SurfaceInst = surface;

        SwapChainSupportDetails swapChainSupport = Utils::GetSwapChainDetails(m_DeviceInst.GetPhysical(), m_SurfaceInst.Get());

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        };

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_SurfaceInst.Get();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = m_DeviceInst.GetQueueFamilies();
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        };

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(m_DeviceInst.Get(), &createInfo, nullptr, &m_SwapChain);

        CORE_ASSERT(result == VK_SUCCESS, "Failed to create swap chain!");

        vkGetSwapchainImagesKHR(m_DeviceInst.Get(), m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_DeviceInst.Get(), m_SwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;

        CreateImageViews();
    };

    void SwapChain::Destroy()
    {
        DestroyImageViews();
        vkDestroySwapchainKHR(m_DeviceInst.Get(), m_SwapChain, nullptr);
    };

    void SwapChain::CreateImageViews()
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());

        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_DeviceInst.Get(), &createInfo, nullptr, &m_SwapChainImageViews[i]);

            CORE_ASSERT(result == VK_SUCCESS, "Failed to create image views!");
        };
    };

    void SwapChain::DestroyImageViews()
    {
        for (auto imageView : m_SwapChainImageViews)
        {
            vkDestroyImageView(m_DeviceInst.Get(), imageView, nullptr);
        };
    };

    VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == m_Config.format && availableFormat.colorSpace == m_Config.colorSpace)
            {
                return availableFormat;
            };
        };

        return availableFormats[0];
    };

    VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == m_Config.presentMode)
            {
                return availablePresentMode;
            };
        };

        return VK_PRESENT_MODE_FIFO_KHR;
    };

    VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {

        if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            GLFWwindow *win = static_cast<GLFWwindow *>(m_SurfaceInst.GetWindow());
            glfwGetFramebufferSize(win, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        };
    };
}