#pragma once
#include <vector>
#include <glad/gl.h>
#include <oglwrap/textures/texture_3D.h>

namespace gpupt
{

class textureArrayGL {
public:
    textureArrayGL();
    ~textureArrayGL();
    void CreateTextureArray(int width, int height, int layers, bool IsFloat=false);
    void LoadTextureLayer(int layerIndex, const std::vector<uint8_t>& imageData, int width, int height);
    void LoadTextureLayer(int layerIndex, const std::vector<float>& imageData, int width, int height);
    void Bind(int textureUnit = 0);
    void Unbind() const;

    GLuint TextureID = 0;  // Exposed for external compatibility
    bool IsFloat = false;

private:
    gl::Texture2DArray texture_;
};
}