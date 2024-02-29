#pragma once
#include "p3d_window.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace p3d
{
#ifndef NDEBUG
#define VALIDATION_LAYERS_ENABLED
#endif
    
    // Enable all standard validation layers
    const std::vector<const char*> validationLayers = {
    	"VK_LAYER_KHRONOS_validation"
    };

    // NOTE: Swapchain will not work without this extension!
    const std::vector<const char *> deviceExtensions
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    struct SwapChainDetails 
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentationModes;

        bool IsValid()
        {
            return !formats.empty() && !presentationModes.empty();
        }
    };

    struct SwapchainImage 
    {
        VkImage image;
        VkImageView imageView;
    };

    class Renderer
    {
    public:
        static constexpr int WIDTH = 1024;
        static constexpr int HEIGHT = 768;

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentationFamily;

            bool AreValid()
            {
                return graphicsFamily.has_value() && presentationFamily.has_value();
            }
        };

        Renderer();
        ~Renderer();

        void Run();

    private:

#ifdef VALIDATION_LAYERS_ENABLED
        void CreateDebugCallback();
        bool CheckValidationLayerSupport();

        VkDebugReportCallbackEXT debugReportCallback_;
#endif 

        Window window_{ WIDTH, HEIGHT, "Potato 3d" };

        QueueFamilyIndices queueFamilyIndices_;

        VkInstance instance_;

        VkPhysicalDevice physicalDevice_;
        VkDevice logicalDevice_;

        VkQueue graphicsQueue_;
        VkQueue presentationQueue_;

        VkSurfaceKHR surface_;

        VkSwapchainKHR swapchain_;
        SwapChainDetails swapChainDetails_;
        std::vector<SwapchainImage> swapChainImages_;

        VkFormat selectedSwapChainImageFormat_;
        VkExtent2D selectedSwapChainExtent_;

        void CreateVulkanInstance();
        void ConfigurePhysicalDeviceAndSwapChainDetails();
        void ConfigureLogicalDevice();
        void CreateSurface();
        void CreateSwapChain();

        bool CheckInstanceExtensionSupport(std::vector<const char*>& extensionList);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

        QueueFamilyIndices GetGraphicsQueueFamilys(const VkPhysicalDevice& device);
        SwapChainDetails GetSwapChainDetails(const VkPhysicalDevice& device);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    };
}
