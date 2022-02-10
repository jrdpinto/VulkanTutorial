#include "p3d_window.h"

namespace p3d 
{
    Window::Window(int w, int h, std::string name) : width_(w), height_(h), window_name_(name) 
    {
        InitWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void Window::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);
    }
}  // namespace p3d 