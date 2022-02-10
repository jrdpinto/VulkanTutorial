#include "first_app.h"

namespace p3d
{
    void App::Run()
    {
        while (!window_.ShouldClose())
        {
            glfwPollEvents();
        }
    }
}