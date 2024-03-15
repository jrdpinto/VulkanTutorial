#include "Mesh.h"
#include <stdexcept>

Mesh::Mesh(VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<Vertex>& vertices)
{
    vertexCount_ = (int)vertices.size();
    physicalDevice_ = physicalDevice;
    device_ = device;
    CreateVertexBuffer(vertices);
}

Mesh::~Mesh()
{
    DestroyVertexBuffer();
}


Mesh::Mesh(Mesh&& other)
{
    vertexCount_ = other.vertexCount_;
    vertexBuffer_ = other.vertexBuffer_;
    vertexBufferMemory_ = other.vertexBufferMemory_;
    physicalDevice_ = other.physicalDevice_;
    device_ = other.device_;

    other.vertexCount_ = 0;
    other.vertexBuffer_ = VK_NULL_HANDLE;
    other.vertexBufferMemory_ = VK_NULL_HANDLE;
    other.physicalDevice_ = VK_NULL_HANDLE;
    other.device_ = VK_NULL_HANDLE;
}

int Mesh::GetVertexCount()
{
    return vertexCount_;
}

VkBuffer Mesh::GetVertexBuffer()
{
    return vertexBuffer_;
}

void Mesh::DestroyVertexBuffer()
{
    if (vertexBuffer_ && device_ && vertexBufferMemory_)
    {
        vkDestroyBuffer(device_, vertexBuffer_, nullptr);
        vkFreeMemory(device_, vertexBufferMemory_, nullptr);
    }
}

void Mesh::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(Vertex) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    // Buffer will not be simultaneously accessed by multiple queues
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device_, &bufferInfo, nullptr, &vertexBuffer_);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Vertex Buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, vertexBuffer_, &memRequirements);

    VkMemoryAllocateInfo memoryAllocInfo{};
    memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocInfo.allocationSize = memRequirements.size;
    memoryAllocInfo.memoryTypeIndex = FindMemoryTypeIndex(memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    result = vkAllocateMemory(device_, &memoryAllocInfo, nullptr, &vertexBufferMemory_);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Vertex Buffer Memory!");
    }

    // Allocate memory to given vertex buffer
    result = vkBindBufferMemory(device_, vertexBuffer_, vertexBufferMemory_, 0);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate memory to the vertex buffer");
    }

    // MAP VERTEX BUFFER MEMORY
    void* data;
    vkMapMemory(device_, vertexBufferMemory_, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(device_, vertexBufferMemory_);
}

uint32_t Mesh::FindMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties))
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}
