#include "GLTFLoader.h"
#include "Logging/logging.h"
#include "../Libs/json/json.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

static std::unordered_map<std::string, std::weak_ptr<Model>> g_modelCache;

static std::vector<unsigned char> DecodeBase64(const std::string& in)
{
    static const unsigned char d[256] = {
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,62 ,255,255,255,63 ,
        52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,255,255,255,255,255,255,
        255,0  ,1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,10 ,11 ,12 ,13 ,14 ,
        15 ,16 ,17 ,18 ,19 ,20 ,21 ,22 ,23 ,24 ,25 ,255,255,255,255,255,
        255,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,40 ,
        41 ,42 ,43 ,44 ,45 ,46 ,47 ,48 ,49 ,50 ,51 ,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
    };
    std::vector<unsigned char> out;
    size_t len = in.size();
    out.reserve((len/4)*3);
    unsigned char a,b,c,e;
    for (size_t i=0;i<len;) {
        do { if (i>=len) return out; a=d[(unsigned char)in[i++]]; } while (a==255);
        do { if (i>=len) return out; b=d[(unsigned char)in[i++]]; } while (b==255);
        out.push_back((a<<2)|((b&0x30)>>4));
        do { if (i>=len) return out; c=in[i++]; if (c=='=') return out; c=d[c]; } while (c==255);
        out.push_back(((b&0x0F)<<4)|((c&0x3C)>>2));
        do { if (i>=len) return out; e=in[i++]; if (e=='=') return out; e=d[e]; } while (e==255);
        out.push_back(((c&0x03)<<6)|e);
    }
    return out;
}

static std::pair<const unsigned char*, int> GetDataRef(const json& j, const std::vector<std::vector<unsigned char>>& buffers, int accessorIdx)
{
    const auto& accessor = j["accessors"][accessorIdx];
    int bufferViewIdx = accessor["bufferView"];
    const auto& bufferView = j["bufferViews"][bufferViewIdx];
    int bufferIdx = bufferView["buffer"];
    int byteOffset = bufferView.value("byteOffset", 0) + accessor.value("byteOffset", 0);
    return { buffers[bufferIdx].data() + byteOffset, accessor["count"] };
}

std::shared_ptr<Model> GLTFLoader::LoadModel(ID3D11Device* device, const std::string& path)
{
    NOTURNAL_LOG_INFO("GLTF", std::string("LoadModel start: ") + path);
    if (auto it = g_modelCache.find(path); it != g_modelCache.end()) {
        if (auto c = it->second.lock()) return c;
    }

    std::ifstream f(path);
    if (!f.is_open()) { NOTURNAL_LOG_ERROR("GLTF", "Failed to open gltf file"); return nullptr; }

    json j;
    try { f >> j; } catch (...) { NOTURNAL_LOG_ERROR("GLTF", "Failed to parse gltf JSON"); return nullptr; }

    auto model = std::make_shared<Model>();
    std::vector<std::vector<unsigned char>> buffers;

    if (j.contains("buffers")) {
        for (const auto& buf : j["buffers"]) {
            std::string uri = buf["uri"];
            int byteLength = buf["byteLength"];
            std::vector<unsigned char> data(byteLength);
            if (uri.rfind("data:application/octet-stream;base64,", 0) == 0) {
                std::string b64 = uri.substr(33);
                auto decoded = DecodeBase64(b64);
                if (decoded.empty()) { NOTURNAL_LOG_ERROR("GLTF", "Base64 buffer decode failed"); return nullptr; }
                if ((int)decoded.size() < byteLength) decoded.resize(byteLength);
                data = std::move(decoded);
            } else {
                fs::path gltfPath(path);
                fs::path binPath = gltfPath.parent_path() / uri;
                std::ifstream binFile(binPath, std::ios::binary);
                if (!binFile.is_open()) { NOTURNAL_LOG_ERROR("GLTF", (std::string("Missing bin: ") + binPath.string())); return nullptr; }
                if (!binFile.read(reinterpret_cast<char*>(data.data()), byteLength)) { NOTURNAL_LOG_ERROR("GLTF", "Failed reading bin buffer"); return nullptr; }
            }
            buffers.push_back(std::move(data));
        }
    }

    if (j.contains("meshes")) {
        for (const auto& meshJson : j["meshes"]) {
            for (const auto& primitive : meshJson["primitives"]) {
                auto newMesh = std::make_shared<Mesh>();
                if (primitive.contains("indices")) {
                    int indicesIdx = primitive["indices"];
                    auto [data, count] = GetDataRef(j, buffers, indicesIdx);
                    if (!data || count <= 0) { NOTURNAL_LOG_ERROR("GLTF", "Indices accessor invalid"); return nullptr; }
                    const auto& accessor = j["accessors"][indicesIdx];
                    int componentType = accessor["componentType"];
                    std::vector<unsigned int> indices;
                    indices.reserve(count);
                    if (componentType == 5123) {
                        const unsigned short* bufp = reinterpret_cast<const unsigned short*>(data);
                        for (int i = 0; i < count; ++i) indices.push_back(bufp[i]);
                    } else if (componentType == 5125) {
                        const unsigned int* bufp = reinterpret_cast<const unsigned int*>(data);
                        for (int i = 0; i < count; ++i) indices.push_back(bufp[i]);
                    }
                    newMesh->IndexCount = (UINT)indices.size();
                    if (newMesh->IndexCount == 0) return nullptr;
                    D3D11_BUFFER_DESC ibd = {};
                    ibd.Usage = D3D11_USAGE_DEFAULT;
                    ibd.ByteWidth = sizeof(unsigned int) * (UINT)indices.size();
                    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
                    D3D11_SUBRESOURCE_DATA initData = {};
                    initData.pSysMem = indices.data();
                    if (FAILED(device->CreateBuffer(&ibd, &initData, &newMesh->IndexBuffer))) { NOTURNAL_LOG_ERROR("GLTF", "Create IndexBuffer failed"); return nullptr; }
                }

                if (primitive["attributes"].contains("POSITION")) {
                    int posIdx = primitive["attributes"]["POSITION"];
                    auto [posData, posCount] = GetDataRef(j, buffers, posIdx);
                    if (!posData || posCount <= 0) { NOTURNAL_LOG_ERROR("GLTF", "Position accessor invalid"); return nullptr; }
                    const auto& accessorPos = j["accessors"][posIdx];
                    int bufferViewPosIdx = accessorPos["bufferView"];
                    const auto& bufferViewPos = j["bufferViews"][bufferViewPosIdx];
                    int stridePos = bufferViewPos.value("byteStride", 0);

                    std::vector<Vertex> vertices;
                    vertices.resize(posCount);

                    XMFLOAT3 bmin = model->BoundsMin;
                    XMFLOAT3 bmax = model->BoundsMax;

                    if (stridePos > 0) {
                        const unsigned char* p = posData;
                        for (int i = 0; i < posCount; ++i) {
                            vertices[i].Position = *reinterpret_cast<const XMFLOAT3*>(p);
                            p += stridePos;
                            if (i == 0) { bmin = vertices[i].Position; bmax = vertices[i].Position; }
                            bmin.x = (std::min)(bmin.x, vertices[i].Position.x);
                            bmin.y = (std::min)(bmin.y, vertices[i].Position.y);
                            bmin.z = (std::min)(bmin.z, vertices[i].Position.z);
                            bmax.x = (std::max)(bmax.x, vertices[i].Position.x);
                            bmax.y = (std::max)(bmax.y, vertices[i].Position.y);
                            bmax.z = (std::max)(bmax.z, vertices[i].Position.z);
                        }
                    } else {
                        const XMFLOAT3* positions = reinterpret_cast<const XMFLOAT3*>(posData);
                        for (int i = 0; i < posCount; ++i) {
                            vertices[i].Position = positions[i];
                            if (i == 0) { bmin = vertices[i].Position; bmax = vertices[i].Position; }
                            bmin.x = (std::min)(bmin.x, vertices[i].Position.x);
                            bmin.y = (std::min)(bmin.y, vertices[i].Position.y);
                            bmin.z = (std::min)(bmin.z, vertices[i].Position.z);
                            bmax.x = (std::max)(bmax.x, vertices[i].Position.x);
                            bmax.y = (std::max)(bmax.y, vertices[i].Position.y);
                            bmax.z = (std::max)(bmax.z, vertices[i].Position.z);
                        }
                    }

                    model->BoundsMin = bmin;
                    model->BoundsMax = bmax;

                    if (primitive["attributes"].contains("NORMAL")) {
                        int normIdx = primitive["attributes"]["NORMAL"];
                        auto [normData, normCount] = GetDataRef(j, buffers, normIdx);
                        const auto& accessorNorm = j["accessors"][normIdx];
                        int bufferViewNormIdx = accessorNorm["bufferView"];
                        const auto& bufferViewNorm = j["bufferViews"][bufferViewNormIdx];
                        int strideNorm = bufferViewNorm.value("byteStride", 0);
                        if (normData) {
                            if (strideNorm > 0) {
                                const unsigned char* p = normData;
                                for (int i = 0; i < normCount && i < posCount; ++i) {
                                    vertices[i].Normal = *reinterpret_cast<const XMFLOAT3*>(p);
                                    p += strideNorm;
                                }
                            } else {
                                const XMFLOAT3* normals = reinterpret_cast<const XMFLOAT3*>(normData);
                                for (int i = 0; i < normCount && i < posCount; ++i) vertices[i].Normal = normals[i];
                            }
                        }
                    }

                    D3D11_BUFFER_DESC vbd = {};
                    vbd.Usage = D3D11_USAGE_DEFAULT;
                    vbd.ByteWidth = sizeof(Vertex) * (UINT)vertices.size();
                    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                    D3D11_SUBRESOURCE_DATA initData = {};
                    initData.pSysMem = vertices.data();
                    if (FAILED(device->CreateBuffer(&vbd, &initData, &newMesh->VertexBuffer))) { NOTURNAL_LOG_ERROR("GLTF", "Create VertexBuffer failed"); return nullptr; }
                    newMesh->VertexCount = (UINT)vertices.size();
                }

                model->Meshes.push_back(newMesh);
            }
        }
    }

    if (model->Meshes.empty()) { NOTURNAL_LOG_ERROR("GLTF", "No meshes parsed"); return nullptr; }
    g_modelCache[path] = model;
    float mh = model->BoundsMax.y - model->BoundsMin.y;
    NOTURNAL_LOG_INFO("GLTF", std::string("LoadModel success, modelYHeight=") + std::to_string(mh));
    return model;
}

std::shared_ptr<Model> GLTFLoader::ImportGameModel(ID3D11Device* device, const std::string& path)
{
    return LoadModel(device, path);
}