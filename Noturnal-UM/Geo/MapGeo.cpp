#include "MapGeo.h"
#include <algorithm>
#include <array>

using namespace Geo;

static DWORD64 read_qword(DWORD64 addr)
{
    DWORD64 v = 0;
    memoryManager.ReadMemory<DWORD64>(addr, v);
    return v;
}

bool MapGeo::Initialize()
{
    pid_ = memoryManager.GetProcessID(L"cs2.exe");
    proc_ = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid_);
    if (!proc_) return false;
    if (!locateBSPInMemory()) return false;
    if (!parseLumps()) return false;
    buildTriangles();
    return !triangles_.empty();
}

bool MapGeo::IsReady() const
{
    return bspBase_ != 0 && !triangles_.empty();
}

void MapGeo::Reset()
{
    vertices_.clear();
    edges_.clear();
    surfedges_.clear();
    faces_.clear();
    triangles_.clear();
    bspBase_ = 0;
    bspSize_ = 0;
    if (proc_) { CloseHandle(proc_); proc_ = nullptr; }
}

bool MapGeo::locateBSPInMemory()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    MEMORY_BASIC_INFORMATION mbi;
    unsigned char buf[4];
    DWORD64 address = (DWORD64)si.lpMinimumApplicationAddress;
    DWORD64 end = (DWORD64)si.lpMaximumApplicationAddress;
    while (address < end)
    {
        if (!VirtualQueryEx(proc_, (LPCVOID)address, &mbi, sizeof(mbi)))
        {
            address += 0x1000;
            continue;
        }
        bool readable = (mbi.State == MEM_COMMIT) && (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE));
        if (readable)
        {
            size_t region = mbi.RegionSize;
            size_t step = 0x4000;
            for (size_t off = 0; off + 4 <= region; off += step)
            {
                DWORD64 probe = (DWORD64)mbi.BaseAddress + off;
                if (!memoryManager.ReadMemory<unsigned char>(probe, buf[0])) continue;
                if (!memoryManager.ReadMemory<unsigned char>(probe + 1, buf[1])) continue;
                if (!memoryManager.ReadMemory<unsigned char>(probe + 2, buf[2])) continue;
                if (!memoryManager.ReadMemory<unsigned char>(probe + 3, buf[3])) continue;
                if (buf[0] == 'V' && buf[1] == 'B' && buf[2] == 'S' && buf[3] == 'P')
                {
                    BSPHeader hdr{};
                    if (!memoryManager.ReadMemory<BSPHeader>(probe, hdr)) { address += (DWORD64)mbi.RegionSize; break; }
                    bspBase_ = probe;
                    bspSize_ = (size_t)hdr.lumps[0].fileofs + (size_t)hdr.lumps[0].filelen;
                    return true;
                }
            }
        }
        address += (DWORD64)mbi.RegionSize;
    }
    return false;
}

bool MapGeo::readLump(int idx, void* dst, size_t len)
{
    BSPHeader hdr{};
    if (!memoryManager.ReadMemory<BSPHeader>(bspBase_, hdr)) return false;
    DWORD64 base = bspBase_ + hdr.lumps[idx].fileofs;
    return memoryManager.ReadMemory<char>(base, *(char*)dst, len);
}

bool MapGeo::parseLumps()
{
    BSPHeader hdr{};
    if (!memoryManager.ReadMemory<BSPHeader>(bspBase_, hdr)) return false;
    int vOfs = hdr.lumps[3].fileofs;
    int vLen = hdr.lumps[3].filelen;
    int eOfs = hdr.lumps[12].fileofs;
    int eLen = hdr.lumps[12].filelen;
    int seOfs = hdr.lumps[13].fileofs;
    int seLen = hdr.lumps[13].filelen;
    int fOfs = hdr.lumps[7].fileofs;
    int fLen = hdr.lumps[7].filelen;

    vertices_.resize(vLen / sizeof(Vec3));
    edges_.resize(eLen / sizeof(BSPEdge));
    surfedges_.resize(seLen / sizeof(int));
    faces_.resize(fLen / sizeof(BSPFace));

    if (!memoryManager.ReadMemory<char>(bspBase_ + vOfs, *(char*)vertices_.data(), vLen)) return false;
    if (!memoryManager.ReadMemory<char>(bspBase_ + eOfs, *(char*)edges_.data(), eLen)) return false;
    if (!memoryManager.ReadMemory<char>(bspBase_ + seOfs, *(char*)surfedges_.data(), seLen)) return false;
    if (!memoryManager.ReadMemory<char>(bspBase_ + fOfs, *(char*)faces_.data(), fLen)) return false;
    return true;
}

void MapGeo::buildTriangles()
{
    triangles_.clear();
    triangles_.reserve(faces_.size() * 2);
    for (const auto& face : faces_)
    {
        if (face.numEdges < 3) continue;
        std::vector<int> verts;
        verts.reserve(face.numEdges);
        for (int i = 0; i < face.numEdges; ++i)
        {
            int se = surfedges_[face.firstEdge + i];
            int ei = std::abs(se);
            const auto& ed = edges_[ei];
            int vi = se >= 0 ? ed.v[0] : ed.v[1];
            verts.push_back(vi);
        }
        int v0 = verts[0];
        for (size_t i = 1; i + 1 < verts.size(); ++i)
        {
            int v1 = verts[i];
            int v2 = verts[i + 1];
            triangles_.push_back({ v0, v1, v2 });
        }
    }
}

bool MapGeo::intersectTriangle(const Vec3& orig, const Vec3& dir, const Vec3& v0, const Vec3& v1, const Vec3& v2, float& t)
{
    const float eps = 1e-5f;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 pvec = Vec3{ dir.y * edge2.z - dir.z * edge2.y, dir.z * edge2.x - dir.x * edge2.z, dir.x * edge2.y - dir.y * edge2.x };
    float det = edge1.x * pvec.x + edge1.y * pvec.y + edge1.z * pvec.z;
    if (det > -eps && det < eps) return false;
    float invDet = 1.0f / det;
    Vec3 tvec = orig - v0;
    float u = (tvec.x * pvec.x + tvec.y * pvec.y + tvec.z * pvec.z) * invDet;
    if (u < 0.0f || u > 1.0f) return false;
    Vec3 qvec = Vec3{ tvec.y * edge1.z - tvec.z * edge1.y, tvec.z * edge1.x - tvec.x * edge1.z, tvec.x * edge1.y - tvec.y * edge1.x };
    float v = (dir.x * qvec.x + dir.y * qvec.y + dir.z * qvec.z) * invDet;
    if (v < 0.0f || u + v > 1.0f) return false;
    t = (edge2.x * qvec.x + edge2.y * qvec.y + edge2.z * qvec.z) * invDet;
    return t > eps;
}

bool MapGeo::RaycastLOS(const Vec3& src, const Vec3& dst) const
{
    if (!IsReady()) return false;
    Vec3 dir = dst - src;
    float len = dir.Length();
    if (len <= 0.0f) return true;
    dir = dir / len;
    float nearest = len;
    for (const auto& tri : triangles_)
    {
        float t;
        if (intersectTriangle(src, dir, vertices_[tri[0]], vertices_[tri[1]], vertices_[tri[2]], t))
        {
            if (t < nearest) nearest = t;
            if (nearest < len) return false;
        }
    }
    return true;
}

RayResult MapGeo::Raycast(const Vec3& src, const Vec3& dst) const
{
    RayResult r{ false, dst };
    if (!IsReady()) return r;
    Vec3 dir = dst - src;
    float len = dir.Length();
    if (len <= 0.0f) { r.hit = false; r.point = dst; return r; }
    dir = dir / len;
    float nearest = len;
    bool hitAny = false;
    for (const auto& tri : triangles_)
    {
        float t;
        if (intersectTriangle(src, dir, vertices_[tri[0]], vertices_[tri[1]], vertices_[tri[2]], t))
        {
            if (t < nearest)
            {
                nearest = t;
                hitAny = true;
            }
        }
    }
    if (hitAny && nearest < len)
    {
        r.hit = true;
        r.point = src + dir * nearest;
    }
    return r;
}
