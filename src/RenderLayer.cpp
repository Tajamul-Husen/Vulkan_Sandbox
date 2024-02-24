#include "RenderLayer.h"
#include "Log.h"

void RenderLayer::OnInit(const AppInstanceData &appInstanceData)
{
    m_Window = appInstanceData.window;

    // Instance and Validation layer
    VulkanCore::InstanceConfig instanceConfig;
    instanceConfig.appName = appInstanceData.title;
    instanceConfig.enableValidation = true;
    instanceConfig.debugCallback = DebugCallback;

    m_VulkanContext.instance.Create(instanceConfig);

    // Surface (Window for rendering)
    m_VulkanContext.surface.Create(m_VulkanContext.instance, m_Window);

    // Device
    VulkanCore::DeviceConfig deviceConfig;
    deviceConfig.requiredExtensions = {};
    deviceConfig.requireGraphicsQueue = true;
    deviceConfig.requirePresentQueue = true;
    deviceConfig.isDiscrete = true;

    m_VulkanContext.device.Create(deviceConfig, m_VulkanContext.instance, m_VulkanContext.surface);

    CORE_LOG_INFO("Device Name: {0}", m_VulkanContext.device.GetDeviceName());

    // SwapChain
    VulkanCore::SwapChainConfig swapChainConfig;
    swapChainConfig.format = VK_FORMAT_B8G8R8A8_SRGB;
    swapChainConfig.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapChainConfig.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    m_VulkanContext.swapChain.Create(swapChainConfig, m_VulkanContext.device, m_VulkanContext.surface);

    // RenderPass
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_VulkanContext.swapChain.GetImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(m_VulkanContext.device.Get(), &renderPassInfo, nullptr, &(m_VulkanContext.renderPass));

    CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass!");

    // Graphics Pipeline
    VulkanCore::ShaderModule vertexShaderModule;
    vertexShaderModule.Create(m_VulkanContext.device, "assets/shaders/spv/shader.vert.spv");

    VulkanCore::ShaderModule fragmentShaderModule;
    fragmentShaderModule.Create(m_VulkanContext.device, "assets/shaders/spv/shader.frag.spv");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexShaderModule.Get();
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentShaderModule.Get();
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // ?Note: Once pipeline is created then it is immutable except dynamic state which are changeable (check supported dynamic state).
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    result = vkCreatePipelineLayout(m_VulkanContext.device.Get(), &pipelineLayoutInfo, nullptr, &(m_VulkanContext.graphicsPipelineLayout));

    CORE_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_VulkanContext.graphicsPipelineLayout;
    pipelineInfo.renderPass = m_VulkanContext.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(m_VulkanContext.device.Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(m_VulkanContext.graphicsPipeline));

    CORE_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline!");

    vertexShaderModule.Destroy();
    fragmentShaderModule.Destroy();

    // FrameBuffer
    m_VulkanContext.swapChainFrameBuffers.resize(m_VulkanContext.swapChain.GetImageViews().size());
    for (size_t i = 0; i < m_VulkanContext.swapChain.GetImageViews().size(); i++)
    {
        VkImageView attachments[] = {m_VulkanContext.swapChain.GetImageViews()[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_VulkanContext.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_VulkanContext.swapChain.GetExtent().width;
        framebufferInfo.height = m_VulkanContext.swapChain.GetExtent().height;
        framebufferInfo.layers = 1;

        result = vkCreateFramebuffer(m_VulkanContext.device.Get(), &framebufferInfo, nullptr, &(m_VulkanContext.swapChainFrameBuffers[i]));

        CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer!");
    };

    // CommandPool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_VulkanContext.device.GetQueueFamilies().graphicsFamily.value();

    result = vkCreateCommandPool(m_VulkanContext.device.Get(), &poolInfo, nullptr, &(m_VulkanContext.commandPool));

    CORE_ASSERT(result == VK_SUCCESS, "Failed to create command pool!");

    // VertexBuffer and StagingBuffer (fast gpu access memory)
    VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(m_VulkanContext.device.Get(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_Vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_VulkanContext.device.Get(), stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

    CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

    vkDestroyBuffer(m_VulkanContext.device.Get(), stagingBuffer, nullptr);
    vkFreeMemory(m_VulkanContext.device.Get(), stagingBufferMemory, nullptr);

    // CommandBuffer
    m_VulkanContext.commandBuffers.resize(m_MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_VulkanContext.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_VulkanContext.commandBuffers.size();

    result = vkAllocateCommandBuffers(m_VulkanContext.device.Get(), &allocInfo, m_VulkanContext.commandBuffers.data());

    CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate command buffers!");

    // Sync Primitives
    m_VulkanContext.imageAvailableSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
    m_VulkanContext.renderFinishedSemaphores.resize(m_MAX_FRAMES_IN_FLIGHT);
    m_VulkanContext.inFlightFences.resize(m_MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(m_VulkanContext.device.Get(), &semaphoreInfo, nullptr, &(m_VulkanContext.imageAvailableSemaphores[i])) != VK_SUCCESS ||
            vkCreateSemaphore(m_VulkanContext.device.Get(), &semaphoreInfo, nullptr, &(m_VulkanContext.renderFinishedSemaphores[i])) != VK_SUCCESS ||
            vkCreateFence(m_VulkanContext.device.Get(), &fenceInfo, nullptr, &(m_VulkanContext.inFlightFences[i])) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        };
    };
};

void RenderLayer::OnPrepareFrame()
{
    vkWaitForFences(m_VulkanContext.device.Get(), 1, &(m_VulkanContext.inFlightFences[m_CurrentFrame]), VK_TRUE, UINT64_MAX);
    vkResetFences(m_VulkanContext.device.Get(), 1, &(m_VulkanContext.inFlightFences[m_CurrentFrame]));

    VkResult result = vkAcquireNextImageKHR(m_VulkanContext.device.Get(), m_VulkanContext.swapChain.Get(), UINT64_MAX, m_VulkanContext.imageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentBufferIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    };

    vkResetFences(m_VulkanContext.device.Get(), 1, &(m_VulkanContext.inFlightFences[m_CurrentFrame]));

    vkResetCommandBuffer(m_VulkanContext.commandBuffers[m_CurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    result = vkBeginCommandBuffer(m_VulkanContext.commandBuffers[m_CurrentFrame], &beginInfo);

    CORE_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_VulkanContext.renderPass;
    renderPassInfo.framebuffer = m_VulkanContext.swapChainFrameBuffers[m_CurrentBufferIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_VulkanContext.swapChain.GetExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(m_VulkanContext.commandBuffers[m_CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(m_VulkanContext.commandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_VulkanContext.graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_VulkanContext.swapChain.GetExtent().width;
    viewport.height = (float)m_VulkanContext.swapChain.GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_VulkanContext.commandBuffers[m_CurrentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_VulkanContext.swapChain.GetExtent();
    vkCmdSetScissor(m_VulkanContext.commandBuffers[m_CurrentFrame], 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {m_VertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(m_VulkanContext.commandBuffers[m_CurrentFrame], 0, 1, vertexBuffers, offsets);

    vkCmdDraw(m_VulkanContext.commandBuffers[m_CurrentFrame], static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);

    vkCmdEndRenderPass(m_VulkanContext.commandBuffers[m_CurrentFrame]);

    result = vkEndCommandBuffer(m_VulkanContext.commandBuffers[m_CurrentFrame]);

    CORE_ASSERT(result == VK_SUCCESS, "Failed to record command buffer!");
};

void RenderLayer::OnRenderFrame()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_VulkanContext.imageAvailableSemaphores[m_CurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(m_VulkanContext.commandBuffers[m_CurrentFrame]);

    VkSemaphore signalSemaphores[] = {m_VulkanContext.renderFinishedSemaphores[m_CurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult result = vkQueueSubmit(m_VulkanContext.device.GetGraphicsQueue(), 1, &submitInfo, m_VulkanContext.inFlightFences[m_CurrentFrame]);

    CORE_ASSERT(result == VK_SUCCESS, "Failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_VulkanContext.swapChain.Get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &m_CurrentBufferIndex;

    result = vkQueuePresentKHR(m_VulkanContext.device.GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized)
    {
        m_FramebufferResized = false;
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image!");
    };

    m_CurrentFrame = (m_CurrentFrame + 1) % m_MAX_FRAMES_IN_FLIGHT;
};

void RenderLayer::OnCleanup()
{
    vkDeviceWaitIdle(m_VulkanContext.device.Get());

    for (size_t i = 0; i < m_MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_VulkanContext.device.Get(), m_VulkanContext.imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(m_VulkanContext.device.Get(), m_VulkanContext.renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(m_VulkanContext.device.Get(), m_VulkanContext.inFlightFences[i], nullptr);
    };

    vkDestroyCommandPool(m_VulkanContext.device.Get(), m_VulkanContext.commandPool, nullptr);

    for (auto framebuffer : m_VulkanContext.swapChainFrameBuffers)
    {
        vkDestroyFramebuffer(m_VulkanContext.device.Get(), framebuffer, nullptr);
    };

    vkDestroyPipeline(m_VulkanContext.device.Get(), m_VulkanContext.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_VulkanContext.device.Get(), m_VulkanContext.graphicsPipelineLayout, nullptr);
    vkDestroyRenderPass(m_VulkanContext.device.Get(), m_VulkanContext.renderPass, nullptr);
    vkDestroyBuffer(m_VulkanContext.device.Get(), m_VertexBuffer, nullptr);
    vkFreeMemory(m_VulkanContext.device.Get(), m_VertexBufferMemory, nullptr);

    m_VulkanContext.swapChain.Destroy();
    m_VulkanContext.device.Destroy();
    m_VulkanContext.surface.Destroy();
    m_VulkanContext.instance.Destroy();
};

void RenderLayer::RecreateSwapChain()
{
    int width = 0, height = 0;
    GLFWwindow *win = static_cast<GLFWwindow *>(m_Window);
    glfwGetFramebufferSize(win, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(win, &width, &height);
        glfwWaitEvents();
    };

    vkDeviceWaitIdle(m_VulkanContext.device.Get());

    // Cleanup FrameBuffer
    for (auto framebuffer : m_VulkanContext.swapChainFrameBuffers)
    {
        vkDestroyFramebuffer(m_VulkanContext.device.Get(), framebuffer, nullptr);
    };

    // Cleanup old SwapChain
    m_VulkanContext.swapChain.Destroy();

    // Create New SwapChain
    VulkanCore::SwapChainConfig swapChainConfig;
    swapChainConfig.format = VK_FORMAT_B8G8R8A8_SRGB;
    swapChainConfig.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapChainConfig.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    m_VulkanContext.swapChain.Create(swapChainConfig, m_VulkanContext.device, m_VulkanContext.surface);

    // Create New FrameBuffer
    m_VulkanContext.swapChainFrameBuffers.resize(m_VulkanContext.swapChain.GetImageViews().size());
    for (size_t i = 0; i < m_VulkanContext.swapChain.GetImageViews().size(); i++)
    {
        VkImageView attachments[] = {m_VulkanContext.swapChain.GetImageViews()[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_VulkanContext.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_VulkanContext.swapChain.GetExtent().width;
        framebufferInfo.height = m_VulkanContext.swapChain.GetExtent().height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(m_VulkanContext.device.Get(), &framebufferInfo, nullptr, &(m_VulkanContext.swapChainFrameBuffers[i]));

        CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer!");
    };
};

void RenderLayer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(m_VulkanContext.device.Get(), &bufferInfo, nullptr, &buffer);

    CORE_ASSERT(result == VK_SUCCESS, "Failed to create buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_VulkanContext.device.Get(), buffer, &memRequirements);

    std::optional<uint32_t> memoryTypeIndex = VulkanCore::Utils::FindMemoryType(m_VulkanContext.device.GetPhysical(), memRequirements.memoryTypeBits, properties);

    CORE_ASSERT(memoryTypeIndex.has_value(), "Failed to get memory type index!");

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex.value();

    result = vkAllocateMemory(m_VulkanContext.device.Get(), &allocInfo, nullptr, &bufferMemory);

    CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate buffer memory!");

    vkBindBufferMemory(m_VulkanContext.device.Get(), buffer, bufferMemory, 0);
};

void RenderLayer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_VulkanContext.commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_VulkanContext.device.Get(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_VulkanContext.device.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_VulkanContext.device.GetGraphicsQueue());

    vkFreeCommandBuffers(m_VulkanContext.device.Get(), m_VulkanContext.commandPool, 1, &commandBuffer);
};

void RenderLayer::OnResize(int width, int height)
{
    m_FramebufferResized = true;
};
