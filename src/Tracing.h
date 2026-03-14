#pragma once
#include <memory>
#include "Scene.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace gpupt
{
#define SAMPLING_MODE_BSDF 0
#define SAMPLING_MODE_LIGHT 1
#define SAMPLING_MODE_BOTH 2
#define SAMPLING_MODE_MIS 3

struct scene;
class buffer;

struct tracingParameters
{
    int Batch = 1;
    int Bounces = 3;

    float CurrentCamera = 0.0f;
    float Clamp = 10.0f;

    int SamplingMode = SAMPLING_MODE_MIS;
};

inline tracingParameters GetTracingParameters()
{
    tracingParameters Params;
    Params.Batch = 1;
    Params.Bounces = 3;
    Params.Clamp = 10.0f;
    Params.CurrentCamera = 0.0f;
    Params.SamplingMode = SAMPLING_MODE_MIS;
    return Params;
}

struct materialPoint
{
    glm::vec3 Emission = glm::vec3(0.0f);
    glm::vec3 Colour = glm::vec3(1.0f);
    int MaterialType = 0;
    float Roughness = 0.5f;
    float Metallic = 0.0f;
    float Opacity = 1.0f;

    glm::vec3 ScatteringColour = glm::vec3(1.0f);
    float TransmissionDepth = 0.01f;
    glm::vec3 Density = glm::vec3(0.0f);
    float Anisotropy = 0.0f;
};

struct light 
{
    int Instance = -1;
    int CDFCount = 0;
    int Environment = -1;
    int CDFStart = 0;
};

struct lights
{
    std::vector<light> Lights;
    std::vector<float> LightsCDF;
    void Build(scene *Scene);
    void RemoveInstance(scene *Scene, int InstanceInx);
    void RecreateBuffers();
    light &AddLight();
    std::shared_ptr<buffer> LightsBuffer = nullptr;
    std::shared_ptr<buffer> LightsCDFBuffer = nullptr;
};


}