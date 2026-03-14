#include "GPUTimer.h"
#include <iostream>

namespace gpupt
{

GPUTimer::GPUTimer()
{
}

GPUTimer::~GPUTimer()
{
    Cleanup();
}

void GPUTimer::Init()
{
    // Resources will be created on-demand
}

void GPUTimer::Cleanup()
{
    // Cleanup OpenGL queries
    for (auto& pair : OpenGLTimings)
    {
        OpenGLTimingData& data = pair.second;
        glDeleteQueries(PING_PONG_BUFFERS, data.Queries);
    }
    OpenGLTimings.clear();

    // Cleanup CUDA events
    for (auto& pair : CUDATimings)
    {
        CUDATimingData& data = pair.second;
        for (int i = 0; i < PING_PONG_BUFFERS; i++)
        {
            if (data.StartEvents[i])
            {
                cudaEventDestroy(data.StartEvents[i]);
            }
            if (data.EndEvents[i])
            {
                cudaEventDestroy(data.EndEvents[i]);
            }
        }
    }
    CUDATimings.clear();
}

void GPUTimer::StartOpenGL(const std::string& name)
{
    if (OpenGLTimings.find(name) == OpenGLTimings.end())
    {
        // Create new timing data
        OpenGLTimingData data = {};
        glGenQueries(PING_PONG_BUFFERS, data.Queries);
        OpenGLTimings[name] = data;
    }

    OpenGLTimingData& data = OpenGLTimings[name];
    glBeginQuery(GL_TIME_ELAPSED, data.Queries[data.CurrentBuffer]);
    data.Started = true;
}

void GPUTimer::StopOpenGL(const std::string& name)
{
    auto it = OpenGLTimings.find(name);
    if (it == OpenGLTimings.end() || !it->second.Started)
    {
        throw std::runtime_error("GPUTimer::StopOpenGL: Timing '" + name + "' was not started");
        return;
    }

    glEndQuery(GL_TIME_ELAPSED);
    it->second.Started = false;
}

void GPUTimer::StartCUDA(const std::string& name)
{
    if (CUDATimings.find(name) == CUDATimings.end())
    {
        // Create new timing data
        CUDATimingData data = {};
        for (int i = 0; i < PING_PONG_BUFFERS; i++)
        {
            cudaEventCreate(&data.StartEvents[i]);
            cudaEventCreate(&data.EndEvents[i]);
        }
        CUDATimings[name] = data;
    }

    CUDATimingData& data = CUDATimings[name];
    cudaEventRecord(data.StartEvents[data.CurrentBuffer]);
    data.Started = true;
}

void GPUTimer::StopCUDA(const std::string& name)
{
    auto it = CUDATimings.find(name);
    if (it == CUDATimings.end() || !it->second.Started)
    {
        throw std::runtime_error("GPUTimer::StopCUDA: Timing '" + name + "' was not started");
        return;
    }

    CUDATimingData& data = it->second;
    cudaEventRecord(data.EndEvents[data.CurrentBuffer]);
    data.Started = false;
}

void GPUTimer::Update()
{
    // Update OpenGL timings
    for (auto& pair : OpenGLTimings)
    {
        const std::string& name = pair.first;
        OpenGLTimingData& data = pair.second;

        if (data.Started)
        {
            continue; // Skip if still in progress
        }

        // Read from the previous buffer (ping-pong)
        int readBuffer = 1 - data.CurrentBuffer;

        GLint available = 0;
        glGetQueryObjectiv(data.Queries[readBuffer], GL_QUERY_RESULT_AVAILABLE, &available);

        if (available)
        {
            GLuint64 timeResult = 0;
            glGetQueryObjectui64v(data.Queries[readBuffer], GL_QUERY_RESULT, &timeResult);
            Timings[name] = timeResult * 0.000001f; // Convert nanoseconds to milliseconds
        }
    }

    // Update CUDA timings
    for (auto& pair : CUDATimings)
    {
        const std::string& name = pair.first;
        CUDATimingData& data = pair.second;

        if (data.Started)
        {
            continue; // Skip if still in progress
        }

        // Read from the previous buffer (ping-pong)
        int readBuffer = 1 - data.CurrentBuffer;

        cudaError_t status = cudaEventQuery(data.EndEvents[readBuffer]);
        if (status == cudaSuccess)
        {
            float timeResult = 0.0f;
            cudaEventElapsedTime(&timeResult, data.StartEvents[readBuffer], data.EndEvents[readBuffer]);
            Timings[name] = timeResult;
        }
    }
}

void GPUTimer::AdvanceFrame()
{
    // Advance ping-pong buffers for all timings
    for (auto& pair : OpenGLTimings)
    {
        pair.second.CurrentBuffer = 1 - pair.second.CurrentBuffer;
    }
    for (auto& pair : CUDATimings)
    {
        pair.second.CurrentBuffer = 1 - pair.second.CurrentBuffer;
    }
}

float GPUTimer::GetTime(const std::string& name) const
{
    auto it = Timings.find(name);
    if (it != Timings.end())
    {
        return it->second;
    }
    return 0.0f;
}

bool GPUTimer::HasTiming(const std::string& name) const
{
    return Timings.find(name) != Timings.end();
}

} // namespace gpupt
