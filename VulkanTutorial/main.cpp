#include "renderer.h"
#include "p3d_window.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main()
{
    try 
    {
        p3d::Window window{ 1024, 768, "Potato 3d" };
        p3d::Renderer renderer(window.GetWindow());

        while (!window.ShouldClose())
        {
            glfwPollEvents();
            renderer.Render();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}