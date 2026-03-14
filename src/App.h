#pragma once
#include <memory>
#include "Tracing.h"
#include "CameraController.h"
#include "Timer.h"
#include "SVGF.h"

// #define USE_OPTIX 1
#if USE_OPTIX
#include <optix.h>
#endif

namespace gpupt
{
class window;
class shaderGL;
class uniformBufferGL;
class textureGL;
struct cudaTextureMapping;
struct scene;
class buffer;
class gui;
class framebuffer;


// struct kernelParams {
//     OptixTraversableHandle handle;
//     float4* output_buffer;
//     int image_width;
//     int image_height;
// };

enum class rasterizeOutputs
{
    Position=0,
    Normal=1,
    UV=2,
    Motion=3
};

struct cudaFramebuffer
{
    unsigned long long PositionTexture = 0;
    unsigned long long NormalTexture = 0;
    unsigned long long UVTexture = 0;
    unsigned long long MotionTexture = 0;
};

class application
{
public:
    void Init();
    void Run();
    void Cleanup();

    static application *Get();
    static glm::uvec2 GetSize();

    void OnResize(uint32_t NewWidth, uint32_t NewHeight);


#if USE_OPTIX
    OptixDeviceContext OptixContext = nullptr;
    void CreateSBT();
    OptixShaderBindingTable SBT = {};
    OptixPipeline pipeline = nullptr;
    std::shared_ptr<buffer> KernelParamsBuffer = nullptr;
#endif
private:
    friend class gui;

    bool Inited = false;

    static std::shared_ptr<application> Singleton;
    std::shared_ptr<window> Window = nullptr;

    std::shared_ptr<scene> Scene = nullptr;
    bool ResetRender = false;

    orbitCameraController Controller;
    bool CameraMoved = false;

    tracingParameters Params = {};
    std::shared_ptr<gui> GUI = nullptr;


    int32_t RenderResolution = 0;
    uint32_t RenderWidth = 0;
    uint32_t RenderHeight = 0;
    uint32_t RenderWindowWidth = 0;
    uint32_t RenderWindowHeight = 0;
    float RenderAspectRatio = 1.0f;

    
    enum class SVGFDebugOutputEnum
    {
        FinalOutput,
        RawOutput,
        Normal,
        Motion,
        Position,
        Depth,
        BarycentricCoords,
        TemporalFilter,
        Moments,
        Variance,
        ATrousWaveletFilter
    }SVGFDebugOutput = SVGFDebugOutputEnum::RawOutput;
    bool DebugRasterize=false;
    glm::vec4 DebugTint=glm::vec4(1);

    int SpatialFilterSteps = 3;
    float DepthThreshold = 0.8f;
    float NormalThreshold = 0.9f;
    int HistoryLength = 24;
    float PhiColour = 10.0f;
    float PhiNormal = 128.0f;



    void Rasterize();
    void Trace();
    void TemporalFilter();
    void WaveletFilter();
    void Tonemap();
    void FilterMoments();
    void TAA();

    float Time = 0.0f;


    std::shared_ptr<framebuffer> Framebuffer[2] = {nullptr, nullptr};
    std::shared_ptr<shaderGL> GBufferShader = nullptr;
    // std::shared_ptr<textureGL> TonemapTexture;


    timer Timer;

    std::shared_ptr<buffer> TracingParamsBuffer = nullptr;

    std::shared_ptr<buffer> RenderBuffer[2] = {nullptr, nullptr};
    std::shared_ptr<buffer> MomentsBuffer[2] = {nullptr, nullptr};
    std::shared_ptr<buffer> FilterBuffer[2] = {nullptr, nullptr};
    std::shared_ptr<buffer> HistoryLengthBuffer = nullptr;
    // std::shared_ptr<buffer> TonemapBuffer;
    // std::shared_ptr<buffer> DenoisedBuffer;
    std::shared_ptr<textureGL> RenderTexture = nullptr;
    std::shared_ptr<cudaTextureMapping> RenderTextureMapping = nullptr;

    uint32_t OutputTexture = 0;

    
    int PingPongInx=0;


    void Render();
    void SaveRender(std::string ImagePath);
    void InitGpuObjects();
    void InitImGui();
    void ResizeRenderTextures();
    void CalculateWindowSizes();
    void StartFrame();
    void EndFrame();
};

}