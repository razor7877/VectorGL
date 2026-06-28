#pragma once

#include <functional>

#include "glad.h"
#include "GLFW/glfw3.h"

namespace Timing
{
    // double measureTime(const std::function<void()>& func)
    // {
    //     double start = glfwGetTime();
    //     func();
    //     // This makes sure the GPU finishes running all the calculations for the pass before measuring time
    //     glFinish();
    //     return glfwGetTime() - start;
    // }

    template<typename F>
    double measureTime(F&& func)
    {
        double start = glfwGetTime();
        func();
        // This makes sure the GPU finishes running all the calculations for the pass before measuring time
        glFinish();
        return glfwGetTime() - start;
    }
}
