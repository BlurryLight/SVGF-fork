#pragma once

#include "Scene.h"
#include <memory>
#include <vector>

namespace gpupt
{

class buffer;




struct bvhNode
{
    glm::vec3 AABBMin = glm::vec3(0.0f);
    float LeftChildOrFirst = 0.0f;
    glm::vec3 AABBMax = glm::vec3(0.0f);
    float TriangleCount = 0.0f;
    bool IsLeaf();
};


struct bin
{
    aabb Bounds;
    uint32_t TrianglesCount=0;
};


struct shape;

struct blas
{
    blas(shape *Shape);
    void Build();
    void Refit();

    void Subdivide(uint32_t NodeIndex);
    void UpdateNodeBounds(uint32_t NodeIndex);
    float FindBestSplitPlane(bvhNode &Node, int &Axis, float &SplitPosition);

    float EvaluateSAH(bvhNode &Node, int Axis, float Position);
    float CalculateNodeCost(bvhNode &Node);

    shape *Shape = nullptr;

    std::vector<uint32_t> TriangleIndices;

    std::vector<bvhNode> BVHNodes;
    uint32_t NodesUsed = 1;
    uint32_t RootNodeIndex = 0;
};





struct tlasNode
{
    glm::vec3 AABBMin = glm::vec3(0.0f);
    uint32_t LeftRight = 0;
    glm::vec3 AABBMax = glm::vec3(0.0f);
    uint32_t BLAS = 0;
    bool IsLeaf() {return LeftRight==0;}
};


struct tlas
{
    tlas(std::vector<instance>* Instances);
    tlas();
    void Build();

    int FindBestMatch(std::vector<int>& List, int N, int A);

    //Instances
    std::vector<instance>* BLAS = nullptr;

    std::vector<tlasNode> Nodes;

    uint32_t NodesUsed = 0;
};



struct indexData
{
    uint32_t triangleDataStartInx = 0;
    uint32_t IndicesDataStartInx = 0;
    uint32_t BVHNodeDataStartInx = 0;
    uint32_t TriangleCount = 0;
};

#if USE_OPTIX
struct optixAS
{
    std::vector<OptixInstance> OptixInstances;
    OptixTraversableHandle InstanceASHandle = nullptr;
    std::shared_ptr<buffer> InstanceASBuffer = nullptr;
    std::shared_ptr<buffer> ShapeASHandlesBuffer = nullptr;
    optixAS(scene *Scene);
    scene *Scene = nullptr;
    void Build();
};
#endif

struct sceneBVH
{
    tlas TLAS;
#if USE_OPTIX
    std::shared_ptr<optixAS> OptixAS = nullptr;
#endif

    std::vector<indexData> IndexData;
    std::vector<triangle> AllTriangles;
    std::vector<uint32_t> AllTriangleIndices;
    std::vector<bvhNode> AllBVHNodes;

    std::shared_ptr<buffer> TrianglesBuffer = nullptr;
    std::shared_ptr<buffer> BVHBuffer = nullptr;
    std::shared_ptr<buffer> IndicesBuffer = nullptr;
    std::shared_ptr<buffer> IndexDataBuffer = nullptr;
    std::shared_ptr<buffer> TLASInstancesBuffer = nullptr;
    std::shared_ptr<buffer> TLASNodeBuffer = nullptr;

    void UpdateShape(uint32_t InstanceInx, uint32_t ShapeInx);
    void UpdateMaterial(uint32_t InstanceInx, uint32_t MaterialInx);
    void UpdateTLAS(uint32_t InstanceInx);
    void AddInstance(uint32_t InstanceInx);
    void RemoveInstance(uint32_t InstanceInx);
    void AddShape(uint32_t ShapeInx);

    int SelectedInstance = -1;
    ~sceneBVH();
    void Destroy();
    scene* Scene = nullptr;
};

std::shared_ptr<sceneBVH> CreateBVH(scene* Scene);

}
