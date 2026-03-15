#version 460 core

in vec2 FragUV;

layout(location = 0) out vec4 OutColor;

// Input textures
uniform sampler2D InputTexture;
uniform usampler2D InputTextureUInt;  // For integer formats
uniform int ChannelIndex;             // 0=Position, 1=Normal, 2=UV, 3=Motion

// Unpack half-float bits to float
float unpackHalfFloat(uint bits)
{
    return unpackHalf2x16(bits).x;
}

vec4 unpackUVec4ToVec4(uvec4 packedData)
{
    return vec4(
        unpackHalfFloat(packedData.r),
        unpackHalfFloat(packedData.g),
        unpackHalfFloat(packedData.b),
        unpackHalfFloat(packedData.a)
    );
}

// Visualization helpers
vec3 visualizeNormal(vec3 normal)
{
    // Map [-1, 1] to [0, 1]
    return normal * 0.5 + 0.5;
}

vec3 visualizeBarycentric(vec3 bary)
{
    // Direct RGB mapping for barycentric coordinates
    return bary;
}

vec3 MappingDepthToColor(float depth)
{
    // give some color to depth, for better visualization
    float near = 0.1;
    float far = 50;
    float normalizedDepth = (depth - near) / (far - near);
    return vec3(normalizedDepth);
}

void main()
{
    if (ChannelIndex == 0)  // Position (float format)
    {
        vec4 data = texture(InputTexture, FragUV);
        OutColor = data;
    }
    else if (ChannelIndex == 1)  // Normal (uint format)
    {
        uvec4 Data = texture(InputTextureUInt, FragUV);
        vec4 normal = unpackUVec4ToVec4(Data);
        OutColor = vec4(visualizeNormal(normal.xyz), 1.0);
    }
    else if (ChannelIndex == 2)  // Barycentric/UV (uint format)
    {
        uvec4 Data = texture(InputTextureUInt, FragUV);
        vec4 bary = unpackUVec4ToVec4(Data);
        OutColor = vec4(visualizeBarycentric(bary.xyz), 1.0);
    }
    else if (ChannelIndex == 3)  // Motion
    {
        vec4 data = texture(InputTexture, FragUV);
        OutColor = vec4(data.xy, 0.0, 1.0);
    }
    else if(ChannelIndex == 4)  // Depth (float format)
    {
        vec4 data = texture(InputTexture, FragUV);
        float depth = data.z;   // depth is encdoed in Z channel of motion texture
        OutColor = vec4(MappingDepthToColor(depth), 1.0);  // Grayscale visualization
    }
    else
    {
        OutColor = vec4(1.0, 0.0, 1.0, 1.0);  // Magenta for error
    }
}
