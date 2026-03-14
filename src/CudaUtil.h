#pragma once
#include <cuda_gl_interop.h>
#include <memory>
#include "TextureGL.h"

#include <glm/vec4.hpp>
#include <iostream>

struct cudaArray;
struct cudaGraphicsResource;
class textureGL;

#define CUDA_CHECK_ERROR(err) \
    do { \
        cudaError_t error = err; \
        if (error != cudaSuccess) { \
            std::cout << "CUDA error at " << __FILE__ << ":" << __LINE__ << " - " << cudaGetErrorString(error) << std::endl; \
            assert(false); \
        } \
    } while (0)

namespace gpupt
{
struct cudaTextureMapping
{
    cudaArray* CudaTextureArray;
    cudaGraphicsResource* CudaTextureResource;
    cudaTextureObject_t TexObj;
    bool bMapped = false;
    bool bWriteOnly = false;
    bool bInitialized = false;

    void Init(GLuint TexID, int Width, int Height, uint32_t ElemSize, bool InbWriteOnly =false)
    {
        // 这里 ElemSize 不是“漏用”，而是目前不需要。因为 cudaGraphicsGLRegisterImage + cudaGraphicsSubResourceGetMappedArray 的 array 格式由 GL 纹理的内部格式决定，CUDA 侧不需要用 ElemSize 来描述格式。
        bWriteOnly = InbWriteOnly;
        cudaGraphicsGLRegisterImage(&CudaTextureResource, TexID, GL_TEXTURE_2D, bWriteOnly ? cudaGraphicsRegisterFlagsWriteDiscard :cudaGraphicsRegisterFlagsNone);
        CUDA_CHECK_ERROR(cudaGetLastError());
        bInitialized = true;
    }

    void Map()
    {
        assert(bInitialized);
        if(!bMapped)
        {
            cudaGraphicsMapResources(1, &CudaTextureResource);
            cudaGraphicsSubResourceGetMappedArray(&CudaTextureArray, CudaTextureResource, 0, 0);

            cudaResourceDesc texRes;
            memset(&texRes, 0, sizeof(cudaResourceDesc));
            texRes.resType = cudaResourceTypeArray;
            texRes.res.array.array = CudaTextureArray;
            
            cudaTextureDesc texDesc;
            memset(&texDesc, 0, sizeof(cudaTextureDesc));
            texDesc.readMode = cudaReadModeElementType;
            

            cudaCreateTextureObject(&TexObj, &texRes, &texDesc, nullptr);    
            bMapped = true;
        }
    }

    void Unmap()
    {
        assert(bInitialized);
        if(bMapped)
        {
            cudaGraphicsUnmapResources(1, &CudaTextureResource);
            CudaTextureArray = nullptr;
            cudaDestroyTextureObject(TexObj);
            bMapped = false;
        }
    }

    void Destroy()
    {
        Unmap();
        assert(bMapped == false);
        assert(bInitialized == true);
        cudaGraphicsUnregisterResource(CudaTextureResource);
    }
    ~cudaTextureMapping()
    {
        Destroy();
    }
};

inline std::shared_ptr<cudaTextureMapping> CreateMapping(std::shared_ptr<textureGL> Tex, bool Write=false, bool CreateWithMapping = true)
{
    std::shared_ptr<cudaTextureMapping> Result = std::make_shared<cudaTextureMapping>();
    Result->Init(Tex->TextureID, Tex->Width, Tex->Height, 4 * sizeof(uint8_t), Write);
    CUDA_CHECK_ERROR(cudaGetLastError());

    if(CreateWithMapping)
    {
        Result->Map();
        CUDA_CHECK_ERROR(cudaGetLastError());
    }

    return Result;
}   

inline std::shared_ptr<cudaTextureMapping> CreateMapping(GLuint TexID, int Width, int Height, uint32_t ElemSize, bool bWriteOnly =false, bool CreateWithMapping = true)
{
    CUDA_CHECK_ERROR(cudaGetLastError());

    std::shared_ptr<cudaTextureMapping> Result = std::make_shared<cudaTextureMapping>();


    Result->Init(TexID, Width, Height, ElemSize, bWriteOnly);
    CUDA_CHECK_ERROR(cudaGetLastError());
    if(CreateWithMapping)
    {
        Result->Map();
        CUDA_CHECK_ERROR(cudaGetLastError());
    }
    return Result;
}   
}