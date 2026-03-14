#pragma once
#include <vector>
#include <glad/gl.h>
#include <memory>
#include <oglwrap/framebuffer.h>
#include <oglwrap/textures/texture_2D.h>

namespace gpupt
{
struct cudaTextureMapping;


struct framebufferDescriptor
{
    GLint InternalFormat;
    GLenum Format;
    GLenum Type;
    uint32_t ElemSize;
};

class framebuffer
{
public:
    framebuffer(int Width, int Height, std::vector<framebufferDescriptor> &Descriptors);

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
};

}