#include "Device.h"
#include "Utils.h"
#include "../Log.h"

namespace VulkanCore
{

	void Device::PickPhysical()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance.Get(), &deviceCount, nullptr);

		CORE_ASSERT(deviceCount != 0, "Failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance.Get(), &deviceCount, devices.data());

		for (const auto &device : devices)
		{
			if (IsSuitable(device))
			{
				m_PhysicalDevice = device;
				break;
			};
		};

		CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
	};

	void Device::Create(const DeviceConfig &config, const Instance &instance, const Surface &surface)
	{
		m_DeviceConfig = config;
		m_Instance = instance;
		m_Surface = surface;

		PickPhysical();

		VkDeviceQueueCreateInfo queueCreateInfo{};
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueQueueFamilies = {
			m_QueueFamilies.graphicsFamily.value(),
			m_QueueFamilies.presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		};

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_Extensions.size());
		createInfo.ppEnabledExtensionNames = m_Extensions.data();

		VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);

		CORE_ASSERT(result == VK_SUCCESS, "Failed to create logical device!");

		vkGetDeviceQueue(m_Device, m_QueueFamilies.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_QueueFamilies.presentFamily.value(), 0, &m_PresentQueue);
	};

	void Device::Destroy()
	{
		vkDestroyDevice(m_Device, nullptr);
	};

	bool Device::IsSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties = GetDeviceProperties(device);
		VkPhysicalDeviceFeatures deviceFeatures = GetDeviceFeatures(device);
		std::vector<VkQueueFamilyProperties> queueFamilies = GetQueueFamilies(device);

		if (m_DeviceConfig.isDiscrete && deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return false;
		};

		if (m_DeviceConfig.requireGraphicsQueue)
		{
			std::optional<uint32_t> graphicsIndex = GetQueueIndex(queueFamilies, VK_QUEUE_GRAPHICS_BIT);
			if (!graphicsIndex.has_value())
			{
				return false;
			}

			m_QueueFamilies.graphicsFamily = graphicsIndex.value();
		};

		if (m_DeviceConfig.requirePresentQueue)
		{
			std::optional<uint32_t> presentIndex = GetPresentQueueIndex(queueFamilies, device, m_Surface.Get());
			if (!presentIndex.has_value())
			{
				return false;
			};

			m_QueueFamilies.presentFamily = presentIndex.value();

			// check swapchain support
			std::vector<const char *> requiredExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
			std::vector<VkExtensionProperties> extensions = GetRequiredExtensions(device, requiredExtensions);
			if (requiredExtensions.size() != extensions.size())
			{
				return false;
			};

			m_Extensions = requiredExtensions;

			SwapChainSupportDetails swapChainDetails = Utils::GetSwapChainDetails(device, m_Surface.Get());
			if (swapChainDetails.formats.empty() || swapChainDetails.presentModes.empty())
			{
				return false;
			};
		};

		m_SelectedDeviceName = deviceProperties.deviceName;

		return true;
	}

	VkPhysicalDeviceProperties Device::GetDeviceProperties(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		return deviceProperties;
	};

	VkPhysicalDeviceFeatures Device::GetDeviceFeatures(VkPhysicalDevice device)
	{
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		return deviceFeatures;
	};

	std::vector<VkQueueFamilyProperties> Device::GetQueueFamilies(VkPhysicalDevice device)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		CORE_ASSERT(queueFamilyCount != 0, "Failed to find queue families!");

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		return queueFamilies;
	};

	std::optional<uint32_t> Device::GetPresentQueueIndex(const std::vector<VkQueueFamilyProperties> &queueFamilies, VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		int index = 0;
		for (const auto &queueFamily : queueFamilies)
		{
			// check which queue family have present support.
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &presentSupport);

			if (presentSupport)
			{
				return index;
			};

			index++;
		};

		return {};
	};

	std::optional<uint32_t> Device::GetQueueIndex(const std::vector<VkQueueFamilyProperties> &queueFamilies, VkQueueFlagBits queueFlag)
	{
		int index = 0;
		for (const auto &queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & queueFlag)
			{
				return index;
			};

			index++;
		};
		return {};
	};

	std::vector<VkExtensionProperties> Device::GetRequiredExtensions(VkPhysicalDevice device, const std::vector<const char *> &requiredExtensions)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::vector<VkExtensionProperties> extensions;

		for (const auto &availableExtension : availableExtensions)
		{
			for (const auto &requiredExtension : requiredExtensions)
			{
				if (std::string_view(availableExtension.extensionName) == std::string_view(requiredExtension))
				{
					extensions.push_back(availableExtension);
				};
			};
		};

		return extensions;
	};

};