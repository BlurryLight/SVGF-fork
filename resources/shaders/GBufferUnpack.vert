#version 460 core

layout(location = 0) in vec2 VertexPosition;  // Quad vertices: -1 to 1
layout(location = 1) in vec2 VertexUV;         // UV coordinates: 0 to 1

out vec2 FragUV;

void main()
{
    FragUV = VertexUV;
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
}
