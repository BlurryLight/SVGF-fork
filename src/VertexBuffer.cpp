#include "VertexBuffer.h"
#include "Scene.h"
#include "DebugLabel.h"
#include <oglwrap/vertex_attrib.h>
#include <oglwrap/context/binding.h>

namespace gpupt
{
vertexBuffer::vertexBuffer(scene *Scene, const std::string& name) : Name(name)
{
    Offsets.resize(Scene->Shapes.size() + 1);

    std::vector<vertex> Vertices;
    std::vector<uint32_t> Indices;
    uint32_t TotalVertices=0;
    for(int i=0; i<Scene->Shapes.size(); i++)
    {
        TotalVertices += Scene->Shapes[i].Triangles.size() * 3;
    }

    Vertices.resize(TotalVertices);
    Indices.resize(TotalVertices);

    uint32_t RunningInx=0;
    uint32_t PrimitiveInx=0;
    for(int i=0; i<Scene->Shapes.size(); i++)
    {
        Offsets[i] = RunningInx;

        for(uint32_t j=0; j<Scene->Shapes[i].Triangles.size(); j++)
        {
            uint32_t Inx0 = RunningInx+0;
            {
                glm::vec4 &PosUVX0 = Scene->Shapes[i].Triangles[j].PositionUvX0;
                glm::vec4 &NormalUVY0 = Scene->Shapes[i].Triangles[j].NormalUvY0;
                Vertices[Inx0].Position = PosUVX0;
                Vertices[Inx0].Normal = NormalUVY0;
                Vertices[Inx0].UV = glm::vec2(PosUVX0.w, NormalUVY0.w);
                Indices[Inx0] = Inx0;
                Vertices[Inx0].PrimitiveIndex = PrimitiveInx;
            }

            uint32_t Inx1 = RunningInx+1;
            {
                glm::vec4 &PosUVX1 = Scene->Shapes[i].Triangles[j].PositionUvX1;
                glm::vec4 &NormalUVY1 = Scene->Shapes[i].Triangles[j].NormalUvY1;
                Vertices[Inx1].Position = PosUVX1;
                Vertices[Inx1].Normal = NormalUVY1;
                Vertices[Inx1].UV = glm::vec2(PosUVX1.w, NormalUVY1.w);
                Indices[Inx1] = Inx1;
                Vertices[Inx1].PrimitiveIndex = PrimitiveInx;
            }

            uint32_t Inx2 = RunningInx+2;
            {
                glm::vec4 &PosUVX2 = Scene->Shapes[i].Triangles[j].PositionUvX2;
                glm::vec4 &NormalUVY2 = Scene->Shapes[i].Triangles[j].NormalUvY2;
                Vertices[Inx2].Position = PosUVX2;
                Vertices[Inx2].Normal = NormalUVY2;
                Vertices[Inx2].UV = glm::vec2(PosUVX2.w, NormalUVY2.w);
                Indices[Inx2] = Inx2;
                Vertices[Inx2].PrimitiveIndex = PrimitiveInx;
            }

            PrimitiveInx++;
            RunningInx+=3;
        }
    }

    Offsets[Offsets.size()-1] = RunningInx;

    Count = Indices.size();

    // Setup VAO using oglwrap
    auto scope_vao = gl::MakeTemporaryBind(vao_);
    auto scope_vbo = gl::MakeTemporaryBind(vbo_);
    auto scope_ebo = gl::MakeTemporaryBind(ebo_);

    // Setup VBO using oglwrap
    vbo_.data(Vertices, gl::BufferUsage::kStaticDraw);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(vertex), (void*)(8 * sizeof(float)));

    // gl::VertexAttrib Position(0);
    // Position.pointer(3,

// Setup EBO using oglwrapebo_.data(Indices, gl::BufferUsage::kStaticDraw);

    // Setup EBO using oglwrap
    ebo_.data(Indices, gl::BufferUsage::kStaticDraw);

    // Expose OpenGL IDs for external compatibility
    VAO = vao_.expose();
    VBO = vbo_.expose();
    EBO = ebo_.expose();

    // Set debug labels for RenderDoc/NSight
    std::string baseName = Name.empty() ? "VertexBuffer" : Name;
    DebugLabel::SetVertexArray(VAO, baseName + "_VAO");
    DebugLabel::SetBuffer(VBO, baseName + "_VBO");
    DebugLabel::SetBuffer(EBO, baseName + "_EBO");

}

void vertexBuffer::Draw(uint32_t ShapeIndex)
{
    auto scope_vao = gl::MakeTemporaryBind(vao_);
    auto scope_ebo = gl::MakeTemporaryBind(ebo_);
    auto scope_vbo = gl::MakeTemporaryBind(vbo_);

    uint32_t Count = Offsets[ShapeIndex+1] - Offsets[ShapeIndex];
    glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, (void*)(Offsets[ShapeIndex] * sizeof(uint32_t)));

}

vertexBuffer::~vertexBuffer()
{
    // oglwrap handles automatic cleanup via RAII
    // No manual deletion needed
}

}