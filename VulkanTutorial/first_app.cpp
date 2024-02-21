#include "first_app.h"
#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <stdio.h>

namespace p3d
{
    void App::CreateVulkanInstance()
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

        // Retrieve list of extensions required by glfw
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::printf("GLFW Required extensions \n");
        const char** requiredExtension = glfwExtensions;
        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            size_t extensionHash = extensionNameHasher(std::string(*requiredExtension));
            bool found = supportedVkExtensionsHash.find(extensionHash) != supportedVkExtensionsHash.end();

            std::printf("%s - %s \n", *requiredExtension, found ? "Supported":"Not supported");
            ++requiredExtension;
        }
        std::cout << std::endl;

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    int GetGraphicsQueueFamilyIndex(const VkPhysicalDevice& device)
    {
        int index = -1;

        // Look for queue families and verify that at least one queue family supports VK_QUEUE_GRAPHICS_BIT
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

        for (int i = 0; i < queueFamilyCount; ++i)
        {
            if (queueFamilyList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    void App::ConfigurePhysicalDevice()
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
            int index = GetGraphicsQueueFamilyIndex(device);
            if (index > -1)
            {
                physicalDevice_ = device;
                graphicsQueueFamilyIndex_ = index;

                break;
            }
        }

        if (physicalDevice_ == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    void App::ConfigureLogicalDevice()
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex_;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        
        VkResult result = vkCreateDevice(physicalDevice_, &createInfo, nullptr, 
            &logicalDevice_);
        if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(logicalDevice_, graphicsQueueFamilyIndex_, 0, &graphicsQueue_);
    }

    App::App()
    {
        CreateVulkanInstance();
        ConfigurePhysicalDevice();
        ConfigureLogicalDevice();
    }

    App::~App()
    {
        vkDestroyDevice(logicalDevice_, nullptr);
        vkDestroyInstance(instance_, nullptr);
    }

    void App::Run()
    {
        while (!window_.ShouldClose())
        {
            glfwPollEvents();
        }
    }
}