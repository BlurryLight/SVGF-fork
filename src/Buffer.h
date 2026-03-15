#pragma once
#include <glad/gl.h>

#include <vector>
#include <string>
#include <oglwrap/buffer.h>

namespace gpupt
{
class buffer {
public:
    buffer(size_t dataSize, const void* data = nullptr, const std::string& name = "");
    ~buffer();
    void Destroy();
    void updateData(const void* data, size_t dataSize);
    void updateData(size_t offset, const void* data, size_t dataSize);
    void Reallocate(const void* data, size_t dataSize);

    void *Data = nullptr;
    uint32_t Size = 0;
    std::string Name = "Unamed Cuda Buffer";
};

class bufferGL {
public:
    bufferGL(size_t dataSize, const void* data = nullptr, const std::string& name = "");
    ~bufferGL();
    void Destroy();
    void updateData(const void* data, size_t dataSize);
    void updateData(size_t offset, const void* data, size_t dataSize);
    void Reallocate(const void* data, size_t dataSize);

    GLuint BufferID = 0;  // Exposed for external compatibility

private:
    gl::BufferObject<gl::BufferType::kShaderStorageBuffer> buffer_;
    std::string Name;
};


class uniformBufferGL
{
public:
    uniformBufferGL(size_t dataSize, const void* data = nullptr, const std::string& name = "");
    ~uniformBufferGL();
    void Destroy();
    void updateData(const void* data, size_t dataSize);
    GLuint BufferID = 0;  // Exposed for external compatibility

private:
    gl::BufferObject<gl::BufferType::kUniformBuffer> buffer_;
    std::string Name;
};

}