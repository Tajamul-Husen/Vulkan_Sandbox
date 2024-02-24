#pragma once

#include <vulkan/vulkan.h>

#include "../Common.h"
#include "Types.h"
#include "Device.h"
#include "Surface.h"


namespace VulkanCore
{
    class SwapChain
    {
    public:
        SwapChain() = default;
        ~SwapChain() = default;

        void Create(const SwapChainConfig &config, const Device &device, const Surface &surface);
        void Destroy();

        VkSwapchainKHR Get() { return m_SwapChain; };
        VkFormat GetImageFormat() { return m_SwapChainImageFormat; };
        VkExtent2D GetExtent() { return m_SwapChainExtent; };
        std::vector<VkImageView> GetImageViews() { return m_SwapChainImageViews; };

    private:
        void CreateImageViews();
        void DestroyImageViews();

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    private:
        SwapChainConfig m_Config;
        Device m_DeviceInst;
        Surface m_SurfaceInst;

        VkSwapchainKHR m_SwapChain;
        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;
    };

};