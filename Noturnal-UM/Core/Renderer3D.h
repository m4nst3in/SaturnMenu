#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "GLTFLoader.h"
#include "../OS-ImGui/OS-ImGui_Struct.h"
#include "../Game/Bone.h"

using namespace DirectX;

class Renderer3D
{
public:
    static Renderer3D& Get()
    {
        static Renderer3D instance;
        return instance;
    }

    bool Initialize(ID3D11Device* device);
    void RenderModel(ID3D11DeviceContext* context, std::shared_ptr<Model> model, const XMMATRIX& world, const XMMATRIX& viewProj, const ImVec4& color);
    void RenderCapsule(ID3D11DeviceContext* context, const XMFLOAT3& p0, const XMFLOAT3& p1, float radius, const XMMATRIX& viewProj, const ImVec4& color);
    void RenderSkeletonBody(ID3D11DeviceContext* context, const CBone& bone, const XMMATRIX& viewProj, const ImVec4& color);

private:
    Renderer3D() = default;
    ~Renderer3D();

    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_InputLayout = nullptr;
    ID3D11Buffer* m_ConstantBuffer = nullptr;
    ID3D11RasterizerState* m_RasterizerState = nullptr;
    ID3D11DepthStencilState* m_DepthStencilState = nullptr;
    ID3D11Buffer* m_CapsuleVB = nullptr;
    ID3D11Buffer* m_CapsuleIB = nullptr;
    UINT m_CapsuleIndexCount = 0;

    struct CBData
    {
        XMMATRIX World;
        XMMATRIX ViewProj;
        XMFLOAT4 Color;
    };
};