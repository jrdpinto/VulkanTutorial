#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace p3d 
{
    class Window 
    {
    public:
        Window(int w, int h, std::string name);
        ~Window();

        // To ensure RAII, do not allow copying/assignment. This step ensures that only one
        // window contains the GLFWindow pointer.
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() { return glfwWindowShouldClose(window_); }

    private:
        void InitWindow();

        const int width_;
        const int height_;

        std::string window_name_;
        GLFWwindow* window_;
    };
}