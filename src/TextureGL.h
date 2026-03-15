#pragma once

#include <vector>
#include <string>
#include <glad/gl.h>

namespace gpupt
{



class textureGL {
public:
    enum class channels
    {
        R, 
        RGB,
        RGBA
    };

    enum class types
    {
        Uint8,
        Float,
        Half
    };

    textureGL(int Width, int Height, channels Channel, types Type, const std::string& name = "");
    ~textureGL();
    void Destroy();
    void Download(std::vector<uint8_t> &Output);
    GLuint TextureID = 0;
    int Width = 0;
    int Height = 0;

private:
    std::string Name;
};

}
