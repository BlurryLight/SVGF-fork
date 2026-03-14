#pragma once
#include <string>
#include <unordered_map>
#include <cuda_runtime.h>
#include <glad/gl.h>
#include <memory>

namespace gpupt
{

class GPUTimer
{
public:
    GPUTimer();
    ~GPUTimer();

    // Initialize timer resources
    void Init();

    // Cleanup timer resources
    void Cleanup();

    // Start timing for a named pass (OpenGL)
    void StartOpenGL(const std::string& name);

    // Stop timing for a named pass (OpenGL)
    void StopOpenGL(const std::string& name);

    // Start timing for a named pass (CUDA)
    void StartCUDA(const std::string& name);

    // Stop timing for a named pass (CUDA)
    void StopCUDA(const std::string& name);

    // Update available timing results (call once per frame)
    void Update();

    // Advance to next frame (swap ping-pong buffers)
    void AdvanceFrame();

    // Get timing result for a pass (in milliseconds)
    float GetTime(const std::string& name) const;

    // Get all timing results
    const std::unordered_map<std::string, float>& GetAllTimings() const { return Timings; }

    // Get frame time
    float GetFrameTime() const { return FrameTime; }

    // Set frame time (call at end of frame)
    void SetFrameTime(float time) { FrameTime = FrameTime * 0.9 + 0.1 * time; }

    // Increment frame count
    void IncrementFrameCount() { FrameCount++; }

    // Get frame count
    int GetFrameCount() const { return FrameCount; }

    // Check if a timing is available
    bool HasTiming(const std::string& name) const;

private:
    static const int PING_PONG_BUFFERS = 2;

    // OpenGL timing data
    struct OpenGLTimingData
    {
        GLuint Queries[PING_PONG_BUFFERS];
        int CurrentBuffer = 0;
        bool Started = false;
    };

    // CUDA timing data
    struct CUDATimingData
    {
        cudaEvent_t StartEvents[PING_PONG_BUFFERS];
        cudaEvent_t EndEvents[PING_PONG_BUFFERS];
        int CurrentBuffer = 0;
        bool Started = false;
    };

    std::unordered_map<std::string, OpenGLTimingData> OpenGLTimings;
    std::unordered_map<std::string, CUDATimingData> CUDATimings;
    std::unordered_map<std::string, float> Timings;

    float FrameTime = 0.0f;
    int FrameCount = 0;
};

} // namespace gpupt
