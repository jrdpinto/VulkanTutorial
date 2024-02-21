#include "first_app.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
    try 
    {
        p3d::App app{};
        app.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}