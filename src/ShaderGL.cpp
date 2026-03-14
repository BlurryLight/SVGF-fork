#include "ShaderGL.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "Buffer.h"
#include "TextureArrayGL.h"
#include <glm/gtc/type_ptr.hpp>

namespace gpupt
{
shaderGL::shaderGL(const char* computePath) {
    std::string shaderCode = ReadFile(computePath);

    gl::Shader shader(gl::ShaderType::kComputeShader);
    shader.set_source(shaderCode);
    shader.compile();

    program_.attachShader(shader);
    program_.link();
}

shaderGL::shaderGL(const char* VertexPath, const char *FragmentPath) {
    std::string vShaderCode = ReadFile(VertexPath);
    std::string fShaderCode = ReadFile(FragmentPath);

    gl::Shader vertexShader(gl::ShaderType::kVertexShader);
    vertexShader.set_source(vShaderCode);
    vertexShader.compile();

    gl::Shader fragmentShader(gl::ShaderType::kFragmentShader);
    fragmentShader.set_source(fShaderCode);
    fragmentShader.compile();

    program_.attachShaders(vertexShader, fragmentShader);
    program_.link();
}

shaderGL::~shaderGL()
{
    this->Destroy();
}

void shaderGL::Destroy()
{
    // oglwrap handles automatic cleanup via RAII
}

void shaderGL::Use() {
    gl::Bind(program_);
}

// Utility functions to bind values to the shader
void shaderGL::SetInt(const std::string& name, int value) {
    gl::Uniform<int>(program_, name).set(value);
}

void shaderGL::SetMat4(const std::string& name, glm::mat4 &Matrix) {
    gl::Uniform<glm::mat4>(program_, name).set(Matrix);
}

void shaderGL::SetVec3(const std::string& name, glm::vec3 &Vector) {
    gl::Uniform<glm::vec3>(program_, name).set(Vector);
}

void shaderGL::SetTexture(int ImageUnit, GLuint TextureID, GLenum Access) {
    glBindImageTexture(ImageUnit, TextureID, 0, GL_FALSE, 0, Access, GL_RGBA32F);
}

void shaderGL::SetTexture(int ImageUnit, GLuint TextureID) const {
    glBindTextureUnit(ImageUnit, TextureID);
}

void shaderGL::SetSSBO(std::shared_ptr<bufferGL> Buffer, int BindingPoint)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BindingPoint, Buffer->BufferID);
}

void shaderGL::SetUBO(std::shared_ptr<uniformBufferGL> Buffer, int BindingPoint)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, Buffer->BufferID);
}

void shaderGL::SetTextureArray(std::shared_ptr<textureArrayGL> Texture, int Unit, std::string Name)
{
    glActiveTexture(GL_TEXTURE0 + Unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, Texture->TextureID);
    gl::Uniform<int>(program_, Name).set(Unit);
}

void shaderGL::Dispatch(uint32_t X, uint32_t Y, uint32_t Z)
{
    this->Use();
    glDispatchCompute(X, Y, Z);
}

void shaderGL::Barrier()
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

std::string shaderGL::ReadFile(const char* FilePath) const {
    std::string IncludeIdentifier = "#include ";
    static bool RecursiveCall=false;

    std::string FullSource = "";
    std::ifstream File(FilePath);

    if(!File.is_open())
    {
        std::cout << "Could not open shader File " << FilePath << std::endl;
        return FullSource;
    }

    std::string LineBuffer;
    while(std::getline(File, LineBuffer))
    {
        if(LineBuffer.find(IncludeIdentifier) != LineBuffer.npos)
        {
            LineBuffer.erase(0, IncludeIdentifier.size());
            size_t found = std::string(FilePath).find_last_of("/\\");
            std::string PathWithoutFileName = std::string(FilePath).substr(0, found + 1);
            LineBuffer.insert(0, PathWithoutFileName);
            RecursiveCall = true;
            FullSource += ReadFile(LineBuffer.c_str());
            continue;
        }

        FullSource += LineBuffer + "\n";
    }

    if(RecursiveCall)
    {
        FullSource += "\0";
    }

    File.close();

    return FullSource;
}

// Compile helper method (wrapper around oglwrap shader compilation)
void shaderGL::CompileShader(gl::ShaderType Type, const char* SourceCode) const {
    gl::Shader shader(Type);
    shader.set_source(SourceCode);
    shader.compile();
}
}