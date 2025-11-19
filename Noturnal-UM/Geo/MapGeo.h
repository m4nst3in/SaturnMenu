#pragma once
#include <vector>
#include <optional>
#include <Windows.h>
#include "../Core/MemoryMgr.h"
#include "../OS-ImGui/OS-ImGui_Struct.h"

namespace Geo
{
    struct BSPHeader
    {
        char magic[4];
        int version;
        struct Lump
        {
            int fileofs;
            int filelen;
            int version;
            char fourCC[4];
        } lumps[64];
        int mapRevision;
    };

    struct BSPFace
    {
        unsigned short planenum;
        unsigned char side;
        unsigned char onNode;
        int firstEdge;
        short numEdges;
        short texinfo;
        short dispinfo;
        short surfaceFogVolumeID;
        unsigned char styles[4];
        int lightofs;
        float area;
        int lightmaptextureMinsInLuxels[2];
        int lightmaptextureSizeInLuxels[2];
        int origFace;
        unsigned short numPrims;
        unsigned short firstPrimID;
        unsigned int smoothingGroups;
    };

    struct BSPEdge { unsigned short v[2]; };

    struct RayResult
    {
        bool hit;
        Vec3 point;
    };

    class MapGeo
    {
    public:
        bool Initialize();
        bool IsReady() const;
        void Reset();
        bool RaycastLOS(const Vec3& src, const Vec3& dst) const;
        RayResult Raycast(const Vec3& src, const Vec3& dst) const;

    private:
        HANDLE proc_ = nullptr;
        DWORD pid_ = 0;
        DWORD64 bspBase_ = 0;
        size_t bspSize_ = 0;
        std::vector<Vec3> vertices_;
        std::vector<BSPEdge> edges_;
        std::vector<int> surfedges_;
        std::vector<BSPFace> faces_;
        std::vector<std::array<int,3>> triangles_;

        bool locateBSPInMemory();
        bool readLump(int idx, void* dst, size_t len);
        bool parseLumps();
        void buildTriangles();
        static bool intersectTriangle(const Vec3& orig, const Vec3& dir, const Vec3& v0, const Vec3& v1, const Vec3& v2, float& t);
    };
}

inline Geo::MapGeo gMapGeo;
