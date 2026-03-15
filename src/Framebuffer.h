#pragma once
#include <vector>
#include <string>
#include <glad/gl.h>
#include <memory>
#include <oglwrap/framebuffer.h>
#include <oglwrap/textures/texture_2D.h>

namespace gpupt
{
struct cudaTextureMapping;


struct framebufferDescriptor
{
    GLint InternalFormat = 0;
    GLenum Format = 0;
    GLenum Type = 0;
    uint32_t ElemSize = 0;
    std::string DebugName = "";
};

class framebuffer
{
public:
    framebuffer(int Width, int Height, std::vector<framebufferDescriptor> &Descriptors, const std::string& name = "");

    GLuint GetTexture(int Index);

    void Destroy();

    void Bind();
    void Unbind();

    ~framebuffer();

    int32_t m_Width = -1;
    int32_t m_Height = -1;
    std::vector<framebufferDescriptor> m_Descriptors;

    std::vector<std::shared_ptr<cudaTextureMapping>> CudaMappings;  // CRITICAL: Must preserve for CUDA interop
private:
    gl::FramebufferObject<gl::FramebufferType::kDrawFramebuffer> fbo_;
    std::vector<gl::Texture2D> colorTextures_;  // oglwrap textures for color attachments
    gl::Texture2D depthTexture_;  // oglwrap texture for depth attachment
    std::string Name;
};

}