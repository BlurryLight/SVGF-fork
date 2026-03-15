#include "TextureArrayGL.h"
#include "DebugLabel.h"
#include <oglwrap/context/binding.h>

namespace gpupt
{
    textureArrayGL::textureArrayGL() : TextureID(0) {}

    textureArrayGL::~textureArrayGL() {
        // oglwrap handles automatic cleanup via RAII
    }

    void textureArrayGL::CreateTextureArray(int Width, int Height, int Layers, bool _IsFloat, const std::string& name) {
        this->IsFloat = _IsFloat;
        this->Name = name;

        gl::Bind(texture_);

        // Set texture parameters using oglwrap
        texture_.minFilter(gl::MinFilter::kLinear);
        texture_.magFilter(gl::MagFilter::kLinear);
        texture_.wrapS(gl::WrapMode::kRepeat);
        texture_.wrapT(gl::WrapMode::kRepeat);

        // Determine format based on IsFloat
        gl::PixelDataInternalFormat internalFormat;
        gl::PixelDataFormat format;
        gl::PixelDataType type;

        if(IsFloat)
        {
            internalFormat = gl::PixelDataInternalFormat::kRgba32F;
            format = gl::PixelDataFormat::kRgba;
            type = gl::PixelDataType::kFloat;
        }
        else
        {
            internalFormat = gl::PixelDataInternalFormat::kRgba;
            format = gl::PixelDataFormat::kRgba;
            type = gl::PixelDataType::kUnsignedByte;
        }

        // Allocate storage for the texture array
        texture_.upload(internalFormat, Width, Height, Layers, format, type, nullptr);

        // Expose the OpenGL ID for external compatibility
        TextureID = texture_.expose();

        // Set debug label for RenderDoc/NSight
        if (!Name.empty())
        {
            DebugLabel::SetTexture(TextureID, Name);
        }
        else
        {
            DebugLabel::SetTexture(TextureID, "TextureArrayGL");
        }
    }

    void textureArrayGL::LoadTextureLayer(int layerIndex, const std::vector<uint8_t>& imageData, int Width, int Height) {
        gl::Bind(texture_);
        texture_.subUpload(0, 0, layerIndex, Width, Height, 1,
                          gl::PixelDataFormat::kRgba,
                          gl::PixelDataType::kUnsignedByte,
                          imageData.data());
    }

    void textureArrayGL::LoadTextureLayer(int layerIndex, const std::vector<float>& imageData, int Width, int Height) {
        gl::Bind(texture_);
        texture_.subUpload(0, 0, layerIndex, Width, Height, 1,
                          gl::PixelDataFormat::kRgba,
                          gl::PixelDataType::kFloat,
                          imageData.data());
    }

    void textureArrayGL::Bind(int textureUnit){
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        gl::Bind(texture_);
    }

    void textureArrayGL::Unbind() const {
        gl::Unbind(gl::TextureType::kTexture2DArray);
    }

}