#ifndef RENDERER_H
#define RENDERER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "Utilities.h"

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
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentationFamily;

            bool AreValid()
            {
                return graphicsFamily.has_value() && presentationFamily.has_value();
            }
        };

        Renderer(GLFWwindow* window);
        ~Renderer();

        void Render(float dt);

    private:

#ifdef VALIDATION_LAYERS_ENABLED
        void CreateDebugCallback();
        bool CheckValidationLayerSupport();

        VkDebugReportCallbackEXT debugReportCallback_;
#endif 

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

        // Container for all frame buffers - one for each swap chain image
        std::vector<VkFramebuffer> swapChainFramebuffers_;
        std::vector<VkCommandBuffer> commandBuffers_;

        VkFormat selectedSwapChainImageFormat_;
        VkExtent2D selectedSwapChainExtent_;

        VkPipeline graphicsPipeline_;
        VkPipelineLayout pipelineLayout_;

        VkRenderPass renderPass_;

        VkCommandPool commandPool_;

        std::vector<VkSemaphore> imageAvailable_;
        std::vector<VkSemaphore> renderFinished_;
        std::vector<VkFence> drawFences_;

        int currentFrame_ = 0;

        std::vector<Mesh> meshes_;

        struct ProjectionMatrices
        {
            glm::mat4 perspective;
            glm::mat4 view;
            glm::mat4 model;
        } projectionMatrices_;

        VkDescriptorSetLayout descriptorSetLayout_;

        VkDescriptorPool descriptorPool_;
        std::vector<VkDescriptorSet> descriptorSets_;

        std::vector<VkBuffer> uniformBuffer_;
        std::vector<VkDeviceMemory> uniformBufferMemory_;

        void CreateVulkanInstance();
        void ConfigurePhysicalDeviceAndSwapChainDetails();
        void ConfigureLogicalDevice();
        void CreateSurface(GLFWwindow* window);
        void CreateSwapChain(GLFWwindow* window);
        void ConfigureGraphicsPipeline();
        void ConfigureRenderPass();
        void ConfigureFrameBuffers();
        void ConfigureCommandPool();
        void ConfigureCommandBuffers();
        void GenerateMeshes();

        void ConfigureDescriptorSetLayout();
        void ConfigureUniformBuffers();
        void ConfigureDescriptorPool();
        void ConfigureDescriptorSets();

        void UpdateUniformBuffer(uint32_t imageIndex);

        void RecordCommands();

        bool CheckInstanceExtensionSupport(std::vector<const char*>& extensionList);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

        void InitSynchronisation();

        QueueFamilyIndices GetGraphicsQueueFamilys(const VkPhysicalDevice& device);
        SwapChainDetails GetSwapChainDetails(const VkPhysicalDevice& device);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

        VkShaderModule CreateShaderModule(const std::vector<char>& code);
    };
}

#endif // RENDERER_H
