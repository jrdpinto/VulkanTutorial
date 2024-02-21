#pragma once
#include "p3d_window.h"
#include <vulkan/vulkan.h>

namespace p3d
{
    class App
    {
    public:
        static constexpr int WIDTH = 1024;
        static constexpr int HEIGHT = 768;

        App();
        ~App();
        void Run();

    private:
        Window window_{ WIDTH, HEIGHT, "Potato 3d" };

        VkInstance instance_;
        VkPhysicalDevice physicalDevice_;
        VkDevice logicalDevice_;
        VkQueue graphicsQueue_;
        int graphicsQueueFamilyIndex_ = -1;

        void CreateVulkanInstance();
        void ConfigurePhysicalDevice();
        void ConfigureLogicalDevice();
    };
}
