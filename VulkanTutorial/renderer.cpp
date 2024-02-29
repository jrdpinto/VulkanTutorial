#include "renderer.h"
#include <functional>
#include <iostream>
#include <set>
#include <stdio.h>
#include <vector>

namespace p3d
{
#ifdef VALIDATION_LAYERS_ENABLED 
    static VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
    {
        // vkGetInstanceProcAddr returns a function pointer to the requested function in the requested instance
        // resulting function is cast as a function pointer with the header of "vkCreateDebugReportCallbackEXT"
        auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    
        // If function was found, executre if with given data and return result, otherwise, return error
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    
    static void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback,
        const VkAllocationCallbacks* pAllocator)
    {
        // get function pointer to requested function, then cast to function pointer for vkDestroyDebugReportCallbackEXT
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    
        // If function found, execute
        if (func != nullptr)
        {
            func(instance, callback, pAllocator);
        }
    }

    // Callback function for validation debugging (will be called when validation information record)
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugReportFlagsEXT flags,				// Type of error
        VkDebugReportObjectTypeEXT objType,			// Type of object causing error
        uint64_t obj,								// ID of object
        size_t location, 
        int32_t code, 
        const char * layerPrefix,
        const char * message,						// Validation Information
        void * userData)
    {
        // If validation ERROR, then output error and return failure
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            printf("VALIDATION ERROR: %s\n", message);
            return VK_TRUE;
        }
    
        // If validation WARNING, then output warning and return okay
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            printf("VALIDATION WARNING: %s\n", message);
            return VK_FALSE;
        }
    
        return VK_FALSE;
    }

    void Renderer::CreateDebugCallback()
    {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = DebugCallback;

        if (CreateDebugReportCallbackEXT(instance_, &createInfo, nullptr, &debugReportCallback_) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug callback!");
        }
    }

    bool Renderer::CheckValidationLayerSupport()
    {
        // Get number of validation layers to create vector of appropriate size
        uint32_t validationLayerCount;
        vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

        if (validationLayerCount < validationLayers.size())
        {
            return false;
        }

        std::vector<VkLayerProperties> availableLayers(validationLayerCount);
        vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

        // Check if given Validation Layer is in list of given Validation Layers
        for (const auto &validationLayer : validationLayers)
        {
            bool hasLayer = false;
            for (const auto &availableLayer : availableLayers)
            {
                if (strcmp(validationLayer, availableLayer.layerName) == 0)
                {
                    hasLayer = true;
                    break;
                }
            }

            if (!hasLayer)
            {
                std::cout << "Validation Layer: " << validationLayer << " not found!" << std::endl;
                return false;
            }
        }

        return true;
    }
#endif

    void Renderer::CreateVulkanInstance()
    {
#ifdef VALIDATION_LAYERS_ENABLED 
        if (!CheckValidationLayerSupport())
        {
            throw std::runtime_error("Required Validation Layers not supported!");
        }
#endif

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

        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef VALIDATION_LAYERS_ENABLED
        requiredExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        createInfo.enabledLayerCount = (uint32_t) validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif

        if (!CheckInstanceExtensionSupport(requiredExtensions))
        {
            throw std::runtime_error("Required extension not supported!");
        }

        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

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

    VkImageView Renderer::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = format;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Subresources allow the view to view only a part of an image
        // Which aspect of image to view (e.g. COLOR_BIT for viewing colour)
        viewCreateInfo.subresourceRange.aspectMask = aspectFlags;

        // Start mipmap level to view from
        viewCreateInfo.subresourceRange.baseMipLevel = 0;

        // Number of mipmap levels to view
        viewCreateInfo.subresourceRange.levelCount = 1;	

        // Start array level to view from
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        
        // Number of array levels to view
        viewCreateInfo.subresourceRange.layerCount = 1;

        // Create image view and return it
        VkImageView imageView;
        VkResult result = vkCreateImageView(logicalDevice_, &viewCreateInfo, nullptr, &imageView);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create an Image View!");
        }

        return imageView;
    }

    void Renderer::ConfigurePhysicalDeviceAndSwapChainDetails()
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

            if (extensionsSupported)
            {
                swapChainDetails_ = GetSwapChainDetails(device);
            }

            if (indices.AreValid() && extensionsSupported && swapChainDetails_.IsValid())
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
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        
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

    VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
    {
        // If only 1 format available and is undefined, then this means ALL formats are available (no restrictions)
        if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }
    
        // If restricted, search for optimal format
        for (const auto &format : formats)
        {
            if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) 
                && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }
    
        // If can't find optimal format, then just return first format
        return formats[0];
    }
    
    VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes)
    {
        // Look for Mailbox presentation mode
        for (const auto &presentationMode : presentationModes)
        {
            if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentationMode;
            }
        }
    
        // If can't find, use FIFO as Vulkan spec says it must be present
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & surfaceCapabilities, GLFWwindow* window)
    {
        // If current extent is at numeric limits, then extent can vary. Otherwise, it is the size of the window.
        if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return surfaceCapabilities.currentExtent;
        }
        else
        {
            // If value can vary, need to set manually
    
            // Get window size
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
    
            // Create new extent using window size
            VkExtent2D newExtent = {};
            newExtent.width = static_cast<uint32_t>(width);
            newExtent.height = static_cast<uint32_t>(height);
    
            // Surface also defines max and min, so make sure within boundaries by clamping value
            newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
            newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
    
            return newExtent;
        }
    }

    void Renderer::CreateSwapChain()
    {
        if (!swapChainDetails_.IsValid())
        {
            throw std::runtime_error("Swap chain details are not valid!");
        }

        VkSurfaceFormatKHR surfaceFormat = ChooseBestSurfaceFormat(swapChainDetails_.formats);
        selectedSwapChainImageFormat_ = surfaceFormat.format;
        VkPresentModeKHR presentMode = ChooseBestPresentationMode(swapChainDetails_.presentationModes);
        selectedSwapChainExtent_ = ChooseSwapExtent(swapChainDetails_.surfaceCapabilities, window_.GetWindow());

        VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = surface_;
        swapChainCreateInfo.imageFormat = surfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapChainCreateInfo.presentMode = presentMode;
        swapChainCreateInfo.imageExtent = selectedSwapChainExtent_;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainCreateInfo.preTransform = swapChainDetails_.surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainCreateInfo.clipped = VK_TRUE;

        uint32_t imageCount = swapChainDetails_.surfaceCapabilities.minImageCount + 1;
        uint32_t maxImageCount = swapChainDetails_.surfaceCapabilities.maxImageCount;
        imageCount = std::max(imageCount, maxImageCount > 0 ? maxImageCount : imageCount);
        swapChainCreateInfo.minImageCount = imageCount;

        // Swapchain images must be shared between families when graphics/presentation indices are different
        if (queueFamilyIndices_.graphicsFamily != queueFamilyIndices_.presentationFamily)
        {
            uint32_t queueFamilyIndices[] = {
                (uint32_t)*queueFamilyIndices_.graphicsFamily,
                (uint32_t)*queueFamilyIndices_.presentationFamily
            };

            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapChainCreateInfo.queueFamilyIndexCount = 0;
            swapChainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        // If old swap chain is no longer valid, then set to null handle
        swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(logicalDevice_, &swapChainCreateInfo, nullptr, &swapchain_);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create a Swapchain!");
        }

        uint32_t swapChainImageCount;
        vkGetSwapchainImagesKHR(logicalDevice_, swapchain_, &swapChainImageCount, nullptr);
        std::vector<VkImage> images(swapChainImageCount);
        vkGetSwapchainImagesKHR(logicalDevice_, swapchain_, &swapChainImageCount, images.data());

        for (VkImage& image : images)
        {
            SwapchainImage swapchainImage{image, CreateImageView(image, selectedSwapChainImageFormat_,
                VK_IMAGE_ASPECT_COLOR_BIT)};
            swapChainImages_.push_back(swapchainImage);
        }
    }

    bool Renderer::CheckInstanceExtensionSupport(std::vector<const char*>& requiredExtensions)
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

        std::printf("Required extensions \n");
        for (const char* extensionName : requiredExtensions)
        {
            size_t extensionHash = extensionNameHasher(std::string(extensionName));
            allExtensionsSupported = supportedVkExtensionsHash.find(extensionHash) != supportedVkExtensionsHash.end();

            std::printf("%s - %s \n", extensionName, allExtensionsSupported ? "Supported":"Not supported");
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
#ifdef VALIDATION_LAYERS_ENABLED 
        CreateDebugCallback();
#endif
        CreateSurface();
        ConfigurePhysicalDeviceAndSwapChainDetails();
        ConfigureLogicalDevice();
        CreateSwapChain();
    }

    Renderer::~Renderer()
    {
        for (auto image : swapChainImages_)
        {
            vkDestroyImageView(logicalDevice_, image.imageView, nullptr);
        }
        vkDestroySwapchainKHR(logicalDevice_, swapchain_, nullptr);

        vkDestroyDevice(logicalDevice_, nullptr);
        vkDestroySurfaceKHR(instance_, surface_, nullptr);

#ifdef VALIDATION_LAYERS_ENABLED
        DestroyDebugReportCallbackEXT(instance_, debugReportCallback_, nullptr);
#endif

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