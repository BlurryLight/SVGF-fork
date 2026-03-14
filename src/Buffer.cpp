#include "buffer.h"
#include "oglwrap/buffer-inl.h"

#include <cuda_runtime.h>
#include <cuda_texture_types.h>
#include <device_launch_parameters.h>

namespace gpupt
{

buffer::buffer(size_t DataSize, const void* InitData) {
    this->Size = DataSize;
    cudaMalloc((void**)&this->Data, Size);
    if(InitData != nullptr) cudaMemcpy(this->Data, InitData, Size, cudaMemcpyHostToDevice);
}

void buffer::Reallocate(const void* InitData, size_t DataSize)
{
    this->Size = DataSize;
    cudaFree(Data);
    cudaMalloc((void**)&this->Data, Size);
    if(InitData != nullptr) cudaMemcpy(this->Data, InitData, Size, cudaMemcpyHostToDevice);
}

buffer::~buffer() {
    Destroy();
}

void buffer::Destroy()
{
    cudaFree(Data);
}

void buffer::updateData(const void* data, size_t DataSize) {
    cudaMemcpy(this->Data, data, DataSize, cudaMemcpyHostToDevice);
}

void buffer::updateData(size_t offset, const void* data, size_t DataSize) {
    cudaMemcpy((void*)((uint8_t*)this->Data + offset), data, DataSize, cudaMemcpyHostToDevice);
}

// 


bufferGL::bufferGL(size_t DataSize, const void* InitData) {
    buffer_.data(DataSize, InitData, gl::BufferUsage::kDynamicCopy);
    BufferID = buffer_.expose();
}

void bufferGL::Reallocate(const void* InitData, size_t DataSize)
{
    buffer_.data(DataSize, InitData, gl::BufferUsage::kDynamicCopy);
}

bufferGL::~bufferGL() {
    if(BufferID != (GLuint)-1)
        Destroy();
}

void bufferGL::Destroy()
{
    // oglwrap handles automatic cleanup via RAII
    BufferID = (GLuint)-1;
}

void bufferGL::updateData(const void* data, size_t DataSize) {
    buffer_.subData(0, DataSize, static_cast<const uint8_t*>(data));
}

void bufferGL::updateData(size_t offset, const void* data, size_t DataSize) {
    buffer_.subData(offset, DataSize, static_cast<const uint8_t*>(data));
}



// 

uniformBufferGL::uniformBufferGL(size_t DataSize, const void* data) {
    buffer_.data(DataSize, data, gl::BufferUsage::kDynamicDraw);
    BufferID = buffer_.expose();
    // Bind to uniform buffer point 8 (as in original implementation)
    glBindBufferBase(GL_UNIFORM_BUFFER, 8, BufferID);
}

uniformBufferGL::~uniformBufferGL() {
    if(BufferID != (GLuint)-1)
        Destroy();
}

void uniformBufferGL::Destroy()
{
    // oglwrap handles automatic cleanup via RAII
    BufferID = -1;
}

void uniformBufferGL::updateData(const void* data, size_t DataSize) {
    buffer_.data(DataSize, data, gl::BufferUsage::kDynamicDraw);
}


}