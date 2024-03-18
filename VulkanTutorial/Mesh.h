#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include "Utilities.h"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 col;
};

class Mesh
{
public:
    Mesh() = default;
    Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue,
        VkCommandPool transferCommandPool, const std::vector<Vertex>& vertices, 
        const std::vector<uint32_t>& indices);
    Mesh(Mesh&& other) noexcept;

    int GetVertexCount();
    VkBuffer GetVertexBuffer();

    int GetIndexCount();
    VkBuffer GetIndexBuffer();

    void DestroyBuffers();

    ~Mesh();

private:
    int vertexCount_;
    VkBuffer vertexBuffer_;
    VkDeviceMemory vertexBufferMemory_;

    int indexCount_;
    VkBuffer indexBuffer_;
    VkDeviceMemory indexBufferMemory_;

    VkPhysicalDevice physicalDevice_;
    VkDevice device_;

    template <typename T>
    void CreateGpuBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
        const std::vector<T>& points, VkBufferUsageFlags usageFlags, VkBuffer& buffer,
        VkDeviceMemory& bufferMemory)
    {
        VkDeviceSize bufferSize = sizeof(T)*points.size();
        
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(physicalDevice_, device_, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);
 
        void* data;
        vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, points.data(), (size_t)bufferSize);
        vkUnmapMemory(device_, stagingBufferMemory);

        // Create buffer for data on GPU access only area
        CreateBuffer(physicalDevice_, device_, bufferSize, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer, bufferMemory);

        // Copy from staging buffer to GPU access buffer
        CopyBuffer(device_, transferQueue, transferCommandPool, stagingBuffer, buffer, bufferSize);
    
        // Destroy + Release Staging Buffer resources
        vkDestroyBuffer(device_, stagingBuffer, nullptr);
        vkFreeMemory(device_, stagingBufferMemory, nullptr);
    }
};
#endif // MESH_H
