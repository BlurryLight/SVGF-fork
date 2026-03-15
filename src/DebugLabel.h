#pragma once

#include <string>
#include <glad/gl.h>
#include <cuda_runtime.h>

namespace gpupt
{

// Helper class for setting OpenGL debug labels
// This allows objects to be identified in RenderDoc, NSight, and other debugging tools
class DebugLabel
{
public:
    // Set label for a buffer object
    static void SetBuffer(GLuint bufferID, const std::string& name);

    // Set label for a texture object
    static void SetTexture(GLuint textureID, const std::string& name);

    // Set label for a framebuffer object
    static void SetFramebuffer(GLuint framebufferID, const std::string& name);

    // Set label for a vertex array object
    static void SetVertexArray(GLuint vaoID, const std::string& name);

    // Set label for a program object
    static void SetProgram(GLuint programID, const std::string& name);

    // Set label for a shader object
    static void SetShader(GLuint shaderID, const std::string& name);

    // Set label for a query object
    static void SetQuery(GLuint queryID, const std::string& name);

    // Set label for any OpenGL object
    static void SetObjectLabel(GLenum identifier, GLuint name, const std::string& label);

    // Generate a formatted name with index
    static std::string FormatName(const std::string& baseName, int index);
    static std::string FormatName(const std::string& baseName, const std::string& qualifier);

    // Set label for CUDA event (CUDA 11.2+)
    static void SetCudaEventLabel(cudaEvent_t event, const std::string& label);
};

// RAII-style OpenGL debug group for marking rendering passes in RenderDoc/NSight
class DebugGroup
{
public:
    DebugGroup(const std::string& name, const std::string& message = "");
    ~DebugGroup();

    // Disable copy and move
    DebugGroup(const DebugGroup&) = delete;
    DebugGroup& operator=(const DebugGroup&) = delete;
    DebugGroup(DebugGroup&&) = delete;
    DebugGroup& operator=(DebugGroup&&) = delete;

private:
    std::string Name;
};

} // namespace gpupt

// Convenience macros for even cleaner syntax
#define GPU_OPENGL_DEBUG_GROUP(name) \
    gpupt::DebugGroup CONCAT(debug_group_, __LINE__)(name)

#define GPU_OPENGL_DEBUG_GROUP_MSG(name, message) \
    gpupt::DebugGroup CONCAT(debug_group_, __LINE__)(name, message)

#define GPU_CUDA_EVENT_LABEL(event, name) \
    gpupt::DebugLabel::SetCudaEventLabel(event, name)

// Helper macro to concatenate tokens
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
