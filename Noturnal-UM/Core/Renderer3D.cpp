#include "Renderer3D.h"
#include "Logging/logging.h"
#include "../Game/Bone.h"
#include <algorithm>

static const char* shaderSource = R"(
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix ViewProj;
    float4 Color;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    float4 pos = float4(input.Pos, 1.0f);
    pos = mul(pos, World);
    output.Pos = mul(pos, ViewProj);
    output.Normal = mul(input.Normal, (float3x3)World);
    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float diff = max(dot(normalize(input.Normal), lightDir), 0.4f);
    return float4(Color.rgb * diff, Color.a);
}
)";

Renderer3D::~Renderer3D()
{
    if (m_VertexShader) m_VertexShader->Release();
    if (m_PixelShader) m_PixelShader->Release();
    if (m_InputLayout) m_InputLayout->Release();
    if (m_ConstantBuffer) m_ConstantBuffer->Release();
    if (m_RasterizerState) m_RasterizerState->Release();
    if (m_DepthStencilState) m_DepthStencilState->Release();
}

bool Renderer3D::Initialize(ID3D11Device* device)
{
    NOTURNAL_LOG_INFO("Renderer3D", "Initialize start");
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompile(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "VS", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        NOTURNAL_LOG_ERROR("Renderer3D", "VS compile failed");
        if (errorBlob) errorBlob->Release();
        return false;
    }

    hr = D3DCompile(shaderSource, strlen(shaderSource), nullptr, nullptr, nullptr, "PS", "ps_4_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        NOTURNAL_LOG_ERROR("Renderer3D", "PS compile failed");
        if (vsBlob) vsBlob->Release();
        if (errorBlob) errorBlob->Release();
        return false;
    }

    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_VertexShader))) {
        NOTURNAL_LOG_ERROR("Renderer3D", "CreateVertexShader failed");
    }
    if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PixelShader))) {
        NOTURNAL_LOG_ERROR("Renderer3D", "CreatePixelShader failed");
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    if (FAILED(device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout))) {
        NOTURNAL_LOG_ERROR("Renderer3D", "CreateInputLayout failed");
    }

    vsBlob->Release();
    psBlob->Release();

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBData);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_ConstantBuffer))) {
        NOTURNAL_LOG_ERROR("Renderer3D", "Create ConstantBuffer failed");
    }

    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    if (FAILED(device->CreateRasterizerState(&rd, &m_RasterizerState))) {
        NOTURNAL_LOG_ERROR("Renderer3D", "Create RasterizerState failed");
    }

    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable = FALSE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsd.StencilEnable = FALSE;
    if (FAILED(device->CreateDepthStencilState(&dsd, &m_DepthStencilState))) {
        NOTURNAL_LOG_ERROR("Renderer3D", "Create DepthStencilState failed");
    }

    const int seg = 16;
    std::vector<Vertex> v;
    std::vector<unsigned int> idx;
    // build cylinder (no caps) centered on Z, height=1, radius=1
    for (int i=0;i<seg;++i){
        float a = (2.0f*XM_PI*i)/seg;
        float x = cosf(a);
        float y = sinf(a);
        v.push_back({ XMFLOAT3(x,y,-0.5f), XMFLOAT3(x,y,0) });
        v.push_back({ XMFLOAT3(x,y, 0.5f), XMFLOAT3(x,y,0) });
    }
    for (int i=0;i<seg;++i){
        int i0 = 2*i;
        int i1 = 2*((i+1)%seg);
        // quad as two triangles between ring segment i and i+1
        idx.push_back(i0); idx.push_back(i1); idx.push_back(i1+1);
        idx.push_back(i0); idx.push_back(i1+1); idx.push_back(i0+1);
    }
    D3D11_BUFFER_DESC vbd = {}; vbd.Usage = D3D11_USAGE_DEFAULT; vbd.ByteWidth = sizeof(Vertex)*(UINT)v.size(); vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vinit = {}; vinit.pSysMem = v.data(); device->CreateBuffer(&vbd, &vinit, &m_CapsuleVB);
    D3D11_BUFFER_DESC ibd = {}; ibd.Usage = D3D11_USAGE_DEFAULT; ibd.ByteWidth = sizeof(unsigned int)*(UINT)idx.size(); ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iinit = {}; iinit.pSysMem = idx.data(); device->CreateBuffer(&ibd, &iinit, &m_CapsuleIB);
    m_CapsuleIndexCount = (UINT)idx.size();

    bool ok = m_VertexShader && m_PixelShader && m_InputLayout && m_ConstantBuffer && m_RasterizerState;
    NOTURNAL_LOG_INFO("Renderer3D", ok ? "Initialize success" : "Initialize incomplete");
    return ok;
}

void Renderer3D::RenderModel(ID3D11DeviceContext* context, std::shared_ptr<Model> model, const XMMATRIX& world, const XMMATRIX& viewProj, const ImVec4& color)
{
    if (!model || !m_VertexShader || !m_PixelShader || !m_InputLayout || !m_ConstantBuffer) { NOTURNAL_LOG_WARN("Renderer3D", "RenderModel skipped: state/model invalid"); return; }

    CBData cbData;
    cbData.World = XMMatrixTranspose(world);
    cbData.ViewProj = XMMatrixTranspose(viewProj);
    cbData.Color = XMFLOAT4(color.x, color.y, color.z, color.w);

    context->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cbData, 0, 0);

    context->IASetInputLayout(m_InputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_VertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    context->PSSetShader(m_PixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    context->RSSetState(m_RasterizerState);
    context->OMSetDepthStencilState(m_DepthStencilState, 0);

    for (const auto& mesh : model->Meshes) {
        if (!mesh || !mesh->VertexBuffer) { NOTURNAL_LOG_WARN("Renderer3D", "Skipping mesh: no VB"); continue; }
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &stride, &offset);
        if (mesh->IndexBuffer && mesh->IndexCount > 0) {
            context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
            NOTURNAL_LOG_DEBUG("Renderer3D", std::string("DrawIndexed ") + std::to_string(mesh->IndexCount));
            context->DrawIndexed(mesh->IndexCount, 0, 0);
        } else if (mesh->VertexCount > 0) {
            NOTURNAL_LOG_DEBUG("Renderer3D", std::string("Draw ") + std::to_string(mesh->VertexCount));
            context->Draw(mesh->VertexCount, 0);
        } else {
            NOTURNAL_LOG_WARN("Renderer3D", "Skipping mesh: counts invalid");
        }
    }
}

void Renderer3D::RenderCapsule(ID3D11DeviceContext* context, const XMFLOAT3& p0, const XMFLOAT3& p1, float radius, const XMMATRIX& viewProj, const ImVec4& color)
{
    if (!m_CapsuleVB || !m_CapsuleIB) return;
    XMVECTOR a = XMLoadFloat3(&p0);
    XMVECTOR b = XMLoadFloat3(&p1);
    XMVECTOR d = XMVectorSubtract(b,a);
    float len = XMVectorGetX(XMVector3Length(d));
    if (len < 1e-3f) return;
    XMVECTOR z = XMVectorSet(0,0,1,0);
    XMVECTOR dirN = XMVector3Normalize(d);
    float dot = XMVectorGetX(XMVector3Dot(z, dirN));
    XMMATRIX rot;
    if (dot > 0.999f) rot = XMMatrixIdentity();
    else if (dot < -0.999f) rot = XMMatrixRotationX(XM_PI);
    else {
        XMVECTOR axis = XMVector3Normalize(XMVector3Cross(z, dirN));
        float angle = acosf((std::max)(-1.0f, (std::min)(1.0f, dot)));
        rot = XMMatrixRotationAxis(axis, angle);
    }
    XMVECTOR mid = XMVectorScale(XMVectorAdd(a,b), 0.5f);
    XMMATRIX scl = XMMatrixScaling(radius, radius, len);
    XMMATRIX trs = XMMatrixTranslationFromVector(mid);
    XMMATRIX world = scl * rot * trs;
    CBData cb; cb.World = XMMatrixTranspose(world); cb.ViewProj = XMMatrixTranspose(viewProj); cb.Color = XMFLOAT4(color.x,color.y,color.z,color.w);
    context->UpdateSubresource(m_ConstantBuffer, 0, nullptr, &cb, 0, 0);
    UINT stride = sizeof(Vertex); UINT offset = 0;
    context->IASetInputLayout(m_InputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(m_VertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    context->PSSetShader(m_PixelShader, nullptr, 0);
    context->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
    context->RSSetState(m_RasterizerState);
    context->OMSetDepthStencilState(m_DepthStencilState, 0);
    context->IASetVertexBuffers(0, 1, &m_CapsuleVB, &stride, &offset);
    context->IASetIndexBuffer(m_CapsuleIB, DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(m_CapsuleIndexCount, 0, 0);
}

void Renderer3D::RenderSkeletonBody(ID3D11DeviceContext* context, const CBone& bone, const XMMATRIX& viewProj, const ImVec4& color)
{
    auto add = [&](DWORD a, DWORD b){
        if (a < bone.BonePosList.size() && b < bone.BonePosList.size()) {
            XMFLOAT3 p0{ bone.BonePosList[a].Pos.x, bone.BonePosList[a].Pos.y, bone.BonePosList[a].Pos.z };
            XMFLOAT3 p1{ bone.BonePosList[b].Pos.x, bone.BonePosList[b].Pos.y, bone.BonePosList[b].Pos.z };
            NOTURNAL_LOG_DEBUG("Renderer3D", std::string("Capsule ")+std::to_string(a)+"->"+std::to_string(b));
            RenderCapsule(context, p0, p1, 3.5f, viewProj, color);
        }
    };
    add(BONEINDEX::neck_0, BONEINDEX::spine_3);
    add(BONEINDEX::spine_3, BONEINDEX::spine_2);
    add(BONEINDEX::spine_2, BONEINDEX::spine_1);
    add(BONEINDEX::spine_1, BONEINDEX::spine_0);
    add(BONEINDEX::spine_0, BONEINDEX::pelvis);
    add(BONEINDEX::neck_0, BONEINDEX::arm_upper_L);
    add(BONEINDEX::arm_upper_L, BONEINDEX::arm_lower_L);
    add(BONEINDEX::arm_lower_L, BONEINDEX::hand_L);
    add(BONEINDEX::neck_0, BONEINDEX::arm_upper_R);
    add(BONEINDEX::arm_upper_R, BONEINDEX::arm_lower_R);
    add(BONEINDEX::arm_lower_R, BONEINDEX::hand_R);
    add(BONEINDEX::pelvis, BONEINDEX::leg_upper_L);
    add(BONEINDEX::leg_upper_L, BONEINDEX::leg_lower_L);
    add(BONEINDEX::leg_lower_L, BONEINDEX::ankle_L);
    add(BONEINDEX::pelvis, BONEINDEX::leg_upper_R);
    add(BONEINDEX::leg_upper_R, BONEINDEX::leg_lower_R);
    add(BONEINDEX::leg_lower_R, BONEINDEX::ankle_R);
}