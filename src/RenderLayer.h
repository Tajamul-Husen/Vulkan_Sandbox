#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Vulkan-Core/Types.h"
#include "Vulkan-Core/Instance.h"
#include "Vulkan-Core/Surface.h"
#include "Vulkan-Core/Device.h"
#include "Vulkan-Core/SwapChain.h"
#include "Vulkan-Core/ShaderModule.h"
#include "Vulkan-Core/Utils.h"

#include "Common.h"
#include "Application.h"
#include "Debug.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    };

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    };
};

struct VulkanContext
{
    VulkanCore::Instance instance;
    VulkanCore::Surface surface;
    VulkanCore::Device device;
    VulkanCore::SwapChain swapChain;
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    VkRenderPass renderPass;
    VkPipelineLayout graphicsPipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};

class RenderLayer : public Layer
{
public:
    virtual void OnInit(const AppInstanceData &appData) override;
    virtual void OnPrepareFrame() override;
    virtual void OnRenderFrame() override;
    virtual void OnCleanup() override;
    virtual void OnResize(int width, int height) override;

private:
    void RecreateSwapChain();
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

private:
    VulkanContext m_VulkanContext;
    void *m_Window = nullptr;
    const uint32_t m_MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t m_CurrentFrame = 0;
    uint32_t m_CurrentBufferIndex;
    bool m_FramebufferResized = false;

    const std::vector<Vertex> m_Vertices = {
        {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;
};