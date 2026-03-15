#include "DebugLabel.h"
#include <sstream>
#include <cuda_runtime.h>

namespace gpupt
{

DebugGroup::DebugGroup(const std::string& name, const std::string& message)
    : Name(name)
{
    if (glPushDebugGroup)
    {
        std::string fullMessage = message.empty() ? name : (name + ": " + message);
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0,
                        static_cast<GLsizei>(fullMessage.length()),
                        fullMessage.c_str());
    }
}

DebugGroup::~DebugGroup()
{
    if (glPopDebugGroup)
    {
        glPopDebugGroup();
    }
}

void DebugLabel::SetBuffer(GLuint bufferID, const std::string& name)
{
    if (bufferID != 0 && bufferID != (GLuint)-1)
    {
        SetObjectLabel(GL_BUFFER, bufferID, name);
    }
}

void DebugLabel::SetTexture(GLuint textureID, const std::string& name)
{
    if (textureID != 0 && textureID != (GLuint)-1)
    {
        SetObjectLabel(GL_TEXTURE, textureID, name);
    }
}

void DebugLabel::SetFramebuffer(GLuint framebufferID, const std::string& name)
{
    if (framebufferID != 0 && framebufferID != (GLuint)-1)
    {
        SetObjectLabel(GL_FRAMEBUFFER, framebufferID, name);
    }
}

void DebugLabel::SetVertexArray(GLuint vaoID, const std::string& name)
{
    if (vaoID != 0 && vaoID != (GLuint)-1)
    {
        SetObjectLabel(GL_VERTEX_ARRAY, vaoID, name);
    }
}

void DebugLabel::SetProgram(GLuint programID, const std::string& name)
{
    if (programID != 0 && programID != (GLuint)-1)
    {
        SetObjectLabel(GL_PROGRAM, programID, name);
    }
}

void DebugLabel::SetShader(GLuint shaderID, const std::string& name)
{
    if (shaderID != 0 && shaderID != (GLuint)-1)
    {
        SetObjectLabel(GL_SHADER, shaderID, name);
    }
}

void DebugLabel::SetQuery(GLuint queryID, const std::string& name)
{
    // it seems query doesn;t support name
    // if (queryID != 0 && queryID != (GLuint)-1)
    // {
    //     SetObjectLabel(GL_QUERY, queryID, name);
    // }
}

void DebugLabel::SetObjectLabel(GLenum identifier, GLuint name, const std::string& label)
{
    if (glObjectLabel)
    {
        glObjectLabel(identifier, name, static_cast<GLsizei>(label.length()), label.c_str());
    }
}

std::string DebugLabel::FormatName(const std::string& baseName, int index)
{
    std::ostringstream ss;
    ss << baseName << "_" << index;
    return ss.str();
}

std::string DebugLabel::FormatName(const std::string& baseName, const std::string& qualifier)
{
    return baseName + "_" + qualifier;
}

void DebugLabel::SetCudaEventLabel(cudaEvent_t event, const std::string& label)
{
    // CUDA 11.2+ supports cudaEventLabel
#if CUDA_VERSION >= 11020
    // Note: cudaEventLabel extension may not be available in all CUDA installations
    // This is a placeholder for future implementation when the API becomes standardized
    // Currently, most tools identify events by their variable names instead
    (void)event;
    (void)label;
#else
    (void)event;
    (void)label;
#endif
}

} // namespace gpupt
