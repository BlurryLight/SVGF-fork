#include "Framebuffer.h"
#include "CudaUtil.h"
#include "DebugLabel.h"
#include <oglwrap/context/binding.h>

namespace gpupt
{

framebuffer::framebuffer(int Width, int Height, std::vector<framebufferDescriptor> &Descriptors, const std::string& name) : Name(name)
{
    m_Width = Width;
    m_Height = Height;
    m_Descriptors = Descriptors;

    colorTextures_.resize(Descriptors.size());

    // Bind the framebuffer using oglwrap
    gl::Bind(fbo_);

    // Set debug label for framebuffer
    if (!Name.empty())
    {
        DebugLabel::SetFramebuffer(fbo_.expose(), Name);
    }
    else
    {
        DebugLabel::SetFramebuffer(fbo_.expose(), "Framebuffer");
    }

    std::vector<gl::FramebufferAttachment> Attachments(Descriptors.size());

    // Create and attach color textures using oglwrap
    for(int i=0; i<Descriptors.size(); i++)
    {
        gl::Bind(colorTextures_[i]);
        colorTextures_[i].upload(
            static_cast<gl::PixelDataInternalFormat>(Descriptors[i].InternalFormat),
            Width, Height,
            static_cast<gl::PixelDataFormat>(Descriptors[i].Format),
            static_cast<gl::PixelDataType>(Descriptors[i].Type),
            nullptr
        );
        colorTextures_[i].minFilter(gl::MinFilter::kNearest);
        colorTextures_[i].magFilter(gl::MagFilter::kNearest);

        // Attach texture to framebuffer
        fbo_.attachTexture(static_cast<gl::FramebufferAttachment>(GL_COLOR_ATTACHMENT0 + i), colorTextures_[i]);

        Attachments[i] = static_cast<gl::FramebufferAttachment>(GL_COLOR_ATTACHMENT0 + i);

        // Set debug label for color texture
        std::string textureName;
        if (!Name.empty())
        {
            textureName = DebugLabel::FormatName(Name, "Color" + Descriptors[i].DebugName);
        }
        else
        {
            textureName = DebugLabel::FormatName("Framebuffer", "Color" + Descriptors[i].DebugName);
        }
        DebugLabel::SetTexture(colorTextures_[i].expose(), textureName);
    }

    // Set draw buffers
    glDrawBuffers(Attachments.size(), reinterpret_cast<GLenum*>(Attachments.data()));

    // Create and attach depth texture using oglwrap
    gl::Bind(depthTexture_);
    depthTexture_.upload(
        gl::PixelDataInternalFormat::kDepthComponent,
        Width, Height,
        gl::PixelDataFormat::kDepthComponent,
        gl::PixelDataType::kFloat,
        nullptr
    );
    depthTexture_.minFilter(gl::MinFilter::kNearest);
    depthTexture_.magFilter(gl::MagFilter::kNearest);
    fbo_.attachTexture(gl::FramebufferAttachment::kDepthAttachment, depthTexture_);

    // Set debug label for depth texture
    std::string depthName;
    if (!Name.empty())
    {
        depthName = DebugLabel::FormatName(Name, "Depth");
    }
    else
    {
        depthName = DebugLabel::FormatName("Framebuffer", "Depth");
    }
    DebugLabel::SetTexture(depthTexture_.expose(), depthName);

    // Validate framebuffer
    if(fbo_.status() != gl::FramebufferStatus::kFramebufferComplete) {
        assert(false);
        exit(0);
    }

    gl::Unbind(fbo_);

    // CRITICAL: Create CUDA mappings using the OpenGL texture IDs
    CudaMappings.resize(Descriptors.size());
    for(int i=0; i<CudaMappings.size(); i++)
    {
        // Use expose().handle() to get the underlying OpenGL texture ID for CUDA
        GLuint texID = colorTextures_[i].expose();
        CudaMappings[i] = CreateMapping(texID, Width, Height, Descriptors[i].ElemSize, false, false);
    }

}

void framebuffer::Destroy()
{
    // Clear CUDA mappings first
    CudaMappings = {};

    // oglwrap handles automatic cleanup via RAII
    // No manual deletion of textures or FBO needed
}

framebuffer::~framebuffer()
{
    Destroy();
}


void framebuffer::Bind()
{
    // CRITICAL: Unmap CUDA resources before OpenGL rendering
    for(int i = 0; i < CudaMappings.size(); i++) {
        if (CudaMappings[i]) {
            CudaMappings[i]->Unmap();
        }
    }

    // Bind framebuffer using oglwrap
    gl::Bind(fbo_);
}

void framebuffer::Unbind()
{
    // CRITICAL: Remap CUDA resources after OpenGL rendering
    for(int i = 0; i < CudaMappings.size(); i++) {
        if (CudaMappings[i]) {
            CudaMappings[i]->Map();
        }
    }

    // Unbind framebuffer using oglwrap
    gl::Unbind(fbo_);
}


GLuint framebuffer::GetTexture(int Index)
{
    // Use expose().handle() to get the underlying OpenGL texture ID
    return colorTextures_[Index].expose();
}

}