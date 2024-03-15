#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 col;
};

class Mesh
{
public:
    Mesh() = default;
    Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, const std::vector<Vertex>& vertices);
    Mesh(Mesh&& other);

    int GetVertexCount();
    VkBuffer GetVertexBuffer();

    void DestroyVertexBuffer();

    ~Mesh();

private:
    int vertexCount_;
    VkBuffer vertexBuffer_;
    VkDeviceMemory vertexBufferMemory_;

    VkPhysicalDevice physicalDevice_;
    VkDevice device_;

    void CreateVertexBuffer(const std::vector<Vertex>& vertices);
    uint32_t FindMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties);

};
