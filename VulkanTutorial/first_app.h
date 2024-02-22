#pragma once
#include "p3d_window.h"
#include <vulkan/vulkan.h>
#include <optional>

namespace p3d
{
    class App
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

        App();
        ~App();
        void Run();

    private:
        Window window_{ WIDTH, HEIGHT, "Potato 3d" };

        QueueFamilyIndices queueFamilyIndices_;

        VkInstance instance_;

        VkPhysicalDevice physicalDevice_;
        VkDevice logicalDevice_;

        VkQueue graphicsQueue_;
        VkQueue presentationQueue_;

	    VkSurfaceKHR surface_;

        void CreateVulkanInstance();
        void ConfigurePhysicalDevice();
        void ConfigureLogicalDevice();
        void CreateSurface();

        QueueFamilyIndices GetGraphicsQueueFamilys(const VkPhysicalDevice& device);
    };
}
