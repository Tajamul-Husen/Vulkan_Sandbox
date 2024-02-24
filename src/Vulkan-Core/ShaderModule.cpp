#include "ShaderModule.h"
#include "../Utils.h"
#include "../Log.h"

namespace VulkanCore
{
    void ShaderModule::Create(const Device &device, const std::string &path)
    {
        m_DeviceInst = device;

        auto code = ReadFile(path);

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkResult result = vkCreateShaderModule(m_DeviceInst.Get(), &createInfo, nullptr, &m_ShaderModule);

        CORE_ASSERT(result == VK_SUCCESS, "Failed to create shader module!");
    };

    void ShaderModule::Destroy()
    {
        vkDestroyShaderModule(m_DeviceInst.Get(), m_ShaderModule, nullptr);
    };
};