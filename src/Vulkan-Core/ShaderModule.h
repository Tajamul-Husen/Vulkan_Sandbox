#pragma once

#include <vulkan/vulkan.h>

#include "../Common.h"
#include "Device.h"

namespace VulkanCore
{
    class ShaderModule
    {
    public:
        ShaderModule() = default;
        ~ShaderModule() = default;

        void Create(const Device &device, const std::string &path);
        void Destroy();

        VkShaderModule Get() { return m_ShaderModule; };

    private:
        VkShaderModule m_ShaderModule;
        Device m_DeviceInst;
    };

};