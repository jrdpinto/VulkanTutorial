#include "renderer.h"
#include <iostream>
#include <set>
#include <functional>
#include <stdio.h>

#include "Utilities.h"

namespace p3d
{
    void Renderer::CreateVulkanInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Potato";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.pEngineName = "Potato 3d";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Retrieve list of extensions required by glfw
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        if (!CheckInstanceExtensionSupport(glfwExtensions, glfwExtensionCount))
        {
            throw std::runtime_error("Required extension not supported!");
        }

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    Renderer::QueueFamilyIndices Renderer::GetGraphicsQueueFamilys(const VkPhysicalDevice& device)
    {
        Renderer::QueueFamilyIndices indices;

        // Look for queue families and verify that at least one queue family supports VK_QUEUE_GRAPHICS_BIT
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

        for (uint32_t i = 0; i < queueFamilyCount; ++i)
        {
            VkQueueFamilyProperties queueFamily = queueFamilyList[i];

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            // Check if Queue Family supports presentation
            VkBool32 presentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentationSupport);

            // Check if queue is presentation type (can be both graphics and presentation)
            if (queueFamily.queueCount > 0 && presentationSupport)
            {
                indices.presentationFamily = i;
            }

            if (indices.AreValid())
            {
                break;
            }
        }

        return indices;
    }

    SwapChainDetails Renderer::GetSwapChainDetails(const VkPhysicalDevice& device)
    {
        SwapChainDetails swapChainDetails;

        // Get the surface capabilities for the surface on the given physical device
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &swapChainDetails.surfaceCapabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

        // Get list of formats
        if (formatCount != 0)
        {
            swapChainDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, 
                swapChainDetails.formats.data());
        }

        uint32_t presentationCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentationCount, nullptr);

        // Get list of presentation modes
        if (presentationCount != 0)
        {
            swapChainDetails.presentationModes.resize(presentationCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentationCount, 
                swapChainDetails.presentationModes.data());
        }

        return swapChainDetails;
    }

    void Renderer::ConfigurePhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            QueueFamilyIndices indices = GetGraphicsQueueFamilys(device);
            
            bool extensionsSupported = CheckDeviceExtensionSupport(device);
            bool swapChainValid = false;
            if (extensionsSupported)
            {
                SwapChainDetails swapChainDetails = GetSwapChainDetails(device);
                swapChainValid = !swapChainDetails.formats.empty() && !swapChainDetails.presentationModes.empty();
            }

            if (indices.AreValid() && extensionsSupported && swapChainValid)
            {
                physicalDevice_ = device;
                queueFamilyIndices_ = indices;

                break;
            }
        }

        if (physicalDevice_ == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    void Renderer::ConfigureLogicalDevice()
    {
        // Use a set to ensure that each index is unique. This is important because the same queue can
        // be used for both graphics and presentation
        std::set<uint32_t> queueFamilyIndices = { *(queueFamilyIndices_.graphicsFamily),
            *(queueFamilyIndices_.presentationFamily) };
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        // Queues the logical device needs to create and info to do so
        for (uint32_t queueFamilyIndex : queueFamilyIndices)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = *(queueFamilyIndices_.graphicsFamily);
            queueCreateInfo.queueCount = 1;
            float queuePriority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        
        VkResult result = vkCreateDevice(physicalDevice_, &createInfo, nullptr, 
            &logicalDevice_);
        if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(logicalDevice_, *(queueFamilyIndices_.graphicsFamily), 0, &graphicsQueue_);
        vkGetDeviceQueue(logicalDevice_, *(queueFamilyIndices_.presentationFamily), 0, &presentationQueue_);
    }

    void Renderer::CreateSurface()
    {
        if (glfwCreateWindowSurface(instance_, window_.GetWindow(), nullptr, &surface_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    bool Renderer::CheckInstanceExtensionSupport(const char** requiredExtensions, int count)
    {
        bool allExtensionsSupported = true;

        // Retrieve list of supported extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::hash<std::string> extensionNameHasher;
        std::set<size_t> supportedVkExtensionsHash;
        std::printf("VK supported extensions \n");
        for (VkExtensionProperties& availableExtension : extensions)
        {
            std::printf("%s \n", availableExtension.extensionName);
            supportedVkExtensionsHash.insert(extensionNameHasher(availableExtension.extensionName));
        }
        std::cout << std::endl;

        std::printf("GLFW Required extensions \n");
        for (uint32_t i = 0; i < count && allExtensionsSupported; ++i)
        {
            size_t extensionHash = extensionNameHasher(std::string(*requiredExtensions));
            allExtensionsSupported = supportedVkExtensionsHash.find(extensionHash) != supportedVkExtensionsHash.end();

            std::printf("%s - %s \n", *requiredExtensions, allExtensionsSupported ? "Supported":"Not supported");

            ++requiredExtensions;
        }
        std::cout << std::endl;

        return allExtensionsSupported;
    }

    bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        bool allExtensionsSupported = false;

        // Get device extension count
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        if (extensionCount == 0)
        {
            return false;
        }

        // Populate list of extensions
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

        std::hash<std::string> extensionNameHasher;
        std::set<size_t> supportedDeviceExtensionsHash;
        std::printf("Device supported extensions \n");
        for (VkExtensionProperties& availableExtension : extensions)
        {
            std::printf("%s \n", availableExtension.extensionName);
            supportedDeviceExtensionsHash.insert(extensionNameHasher(availableExtension.extensionName));
        }
        std::cout << std::endl;

        std::printf("Required device extensions \n");
        for (const auto &deviceExtension : deviceExtensions)
        {
            size_t extensionHash = extensionNameHasher(std::string(deviceExtension));
            allExtensionsSupported = supportedDeviceExtensionsHash.find(extensionHash) != supportedDeviceExtensionsHash.end();

            std::printf("%s - %s \n", deviceExtension, allExtensionsSupported ? "Supported":"Not supported");

            if (!allExtensionsSupported)
            {
                break;
            }
        }

        return allExtensionsSupported;
    }

    Renderer::Renderer()
    {
        CreateVulkanInstance();
        CreateSurface();
        ConfigurePhysicalDevice();
        ConfigureLogicalDevice();
    }

    Renderer::~Renderer()
    {
        vkDestroyDevice(logicalDevice_, nullptr);
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        vkDestroyInstance(instance_, nullptr);
    }

    void Renderer::Run()
    {
        while (!window_.ShouldClose())
        {
            glfwPollEvents();
        }
    }
}