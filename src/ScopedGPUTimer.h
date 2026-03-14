#pragma once
#include "GPUTimer.h"
#include <string>

namespace gpupt
{

// RAII-style timer for OpenGL passes
class ScopedOpenGLTimer
{
public:
    ScopedOpenGLTimer(GPUTimer& timer, const std::string& name)
        : Timer(timer), Name(name)
    {
        Timer.StartOpenGL(Name);
    }

    ~ScopedOpenGLTimer()
    {
        try
        {
            Timer.StopOpenGL(Name);
        }
        catch (...)
        {
            // Suppress exceptions in destructor
        }
    }

    // Disable copy and move
    ScopedOpenGLTimer(const ScopedOpenGLTimer&) = delete;
    ScopedOpenGLTimer& operator=(const ScopedOpenGLTimer&) = delete;
    ScopedOpenGLTimer(ScopedOpenGLTimer&&) = delete;
    ScopedOpenGLTimer& operator=(ScopedOpenGLTimer&&) = delete;

private:
    GPUTimer& Timer;
    std::string Name;
};

// RAII-style timer for CUDA passes
class ScopedCUDATimer
{
public:
    ScopedCUDATimer(GPUTimer& timer, const std::string& name)
        : Timer(timer), Name(name)
    {
        Timer.StartCUDA(Name);
    }

    ~ScopedCUDATimer()
    {
        try
        {
            Timer.StopCUDA(Name);
        }
        catch (...)
        {
            // Suppress exceptions in destructor
        }
    }

    // Disable copy and move
    ScopedCUDATimer(const ScopedCUDATimer&) = delete;
    ScopedCUDATimer& operator=(const ScopedCUDATimer&) = delete;
    ScopedCUDATimer(ScopedCUDATimer&&) = delete;
    ScopedCUDATimer& operator=(ScopedCUDATimer&&) = delete;

private:
    GPUTimer& Timer;
    std::string Name;
};

} // namespace gpupt

// Convenience macros for even cleaner syntax
#define GPU_OPENGL_TIMER_SCOPE(timer, name) \
    gpupt::ScopedOpenGLTimer CONCAT(timer_, __LINE__)(timer, name)

#define GPU_CUDA_TIMER_SCOPE(timer, name) \
    gpupt::ScopedCUDATimer CONCAT(timer_, __LINE__)(timer, name)

// Helper macro to concatenate tokens
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
