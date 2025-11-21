#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <memory>

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 Position;
    XMFLOAT3 Normal;
};

struct Mesh
{
    ID3D11Buffer* VertexBuffer = nullptr;
    ID3D11Buffer* IndexBuffer = nullptr;
    UINT IndexCount = 0;
    UINT VertexCount = 0;

    ~Mesh()
    {
        if (VertexBuffer) VertexBuffer->Release();
        if (IndexBuffer) IndexBuffer->Release();
    }
};

struct Model
{
    std::vector<std::shared_ptr<Mesh>> Meshes;
    XMFLOAT3 BoundsMin{ 0,0,0 };
    XMFLOAT3 BoundsMax{ 0,0,0 };
};

class GLTFLoader
{
public:
    static std::shared_ptr<Model> LoadModel(ID3D11Device* device, const std::string& path);
    static std::shared_ptr<Model> ImportGameModel(ID3D11Device* device, const std::string& path);
};