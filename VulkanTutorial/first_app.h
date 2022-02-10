#pragma once
#include "p3d_window.h"

namespace p3d
{
    class App
    {
    public:
        static constexpr int WIDTH = 1024;
        static constexpr int HEIGHT = 768;

        void Run();

    private:
        Window window_{ WIDTH, HEIGHT, "Potato 3d" };
    };
}
