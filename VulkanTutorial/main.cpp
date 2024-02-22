#include "renderer.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    try 
    {
        p3d::Renderer renderer{};
        renderer.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}