#pragma once
#include <vector>
namespace Core {
struct FrameContext;
struct IFeature {
    virtual ~IFeature() {}
    virtual void Init() {}
    virtual void OnFrame(const FrameContext&) = 0;
    virtual void OnRender() {}
    virtual void Shutdown() {}
};
}

