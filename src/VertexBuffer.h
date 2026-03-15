#pragma once
#include <glad/gl.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <oglwrap/vertex_array.h>
#include <oglwrap/buffer.h>

namespace gpupt
{

struct scene;

struct vertex
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Normal = glm::vec3(0.0f);
    glm::vec2 UV = glm::vec2(0.0f);
    uint32_t PrimitiveIndex = 0;
};

struct vertexBuffer
{
public:
    vertexBuffer(scene *Scene, const std::string& name = "");
    ~vertexBuffer();

    void Draw(uint32_t ShapeIndex);

    std::vector<uint32_t> Offsets;

    GLuint VAO = GL_INVALID_VALUE;  // Exposed for external compatibility
    GLuint VBO = GL_INVALID_VALUE;  // Exposed for external compatibility
    GLuint EBO = GL_INVALID_VALUE;  // Exposed for external compatibility

    uint32_t Count=0;

private:
    gl::VertexArray vao_;
    gl::ArrayBuffer vbo_;
    gl::IndexBuffer ebo_;
    std::string Name;
};

}