#include "renderer.h"
#include "p3d_window.h"

#include <iostream>

int main()
{
    try 
    {
        p3d::Window window{ 1024, 768, "Potato 3d" };
        p3d::Renderer renderer(window.GetWindow());

        float deltaTime = 0.0f, prevTime = 0.0f;

        while (!window.ShouldClose())
        {
            glfwPollEvents();

            float now = (float)glfwGetTime();
            deltaTime = now - prevTime;
            prevTime = now;

            renderer.Render(deltaTime);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}