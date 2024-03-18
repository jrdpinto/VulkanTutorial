#include "Mesh.h"

Mesh::Mesh(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transferQueue,
        VkCommandPool transferCommandPool, const std::vector<Vertex>& vertices, 
        const std::vector<uint32_t>& indices)
{
    vertexCount_ = (int)vertices.size();
    indexCount_ = (int)indices.size();
    physicalDevice_ = physicalDevice;
    device_ = device;

    CreateGpuBuffer(transferQueue, transferCommandPool, vertices, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer_,
        vertexBufferMemory_);
    CreateGpuBuffer(transferQueue, transferCommandPool, indices,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer_,
        indexBufferMemory_);
}

Mesh::~Mesh()
{
    DestroyBuffers();
}

Mesh::Mesh(Mesh&& other) noexcept
{
    vertexCount_ = other.vertexCount_;
    vertexBuffer_ = other.vertexBuffer_;
    vertexBufferMemory_ = other.vertexBufferMemory_;
    physicalDevice_ = other.physicalDevice_;
    device_ = other.device_;
    indexCount_ = other.indexCount_;
    indexBufferMemory_ = other.indexBufferMemory_;
    indexBuffer_ = other.indexBuffer_;

    other.vertexCount_ = 0;
    other.vertexBuffer_ = VK_NULL_HANDLE;
    other.indexCount_ = 0;
    other.indexBuffer_ = VK_NULL_HANDLE;
    other.indexBufferMemory_ = VK_NULL_HANDLE;
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

int Mesh::GetIndexCount()
{
    return indexCount_;
}

VkBuffer Mesh::GetIndexBuffer()
{
    return indexBuffer_;
}

void Mesh::DestroyBuffers()
{
    if (device_)
    {
        vkDestroyBuffer(device_, vertexBuffer_, nullptr);
        vkFreeMemory(device_, vertexBufferMemory_, nullptr);
        vkDestroyBuffer(device_, indexBuffer_, nullptr);
        vkFreeMemory(device_, indexBufferMemory_, nullptr);
    }
}
