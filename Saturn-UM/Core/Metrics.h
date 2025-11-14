#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
namespace Core {
class Metrics {
public:
    void Record(const std::string& name, long long ms) { if (paused) return; last[name] = ms; auto& v = hist[name]; v.push_back(ms); if (v.size() > 60) v.erase(v.begin()); }
    long long Get(const std::string& name) const { auto it = last.find(name); return it==last.end()?0:it->second; }
    long long Average(const std::string& name) const { auto it = hist.find(name); if (it==hist.end() || it->second.empty()) return 0; long long s = std::accumulate(it->second.begin(), it->second.end(), 0LL); return s / (long long)it->second.size(); }
    long long Median(const std::string& name) const { auto it = hist.find(name); if (it==hist.end() || it->second.empty()) return 0; auto v = it->second; size_t n = v.size(); std::nth_element(v.begin(), v.begin()+n/2, v.end()); return v[n/2]; }
    long long P95(const std::string& name) const { auto it = hist.find(name); if (it==hist.end() || it->second.empty()) return 0; auto v = it->second; std::sort(v.begin(), v.end()); size_t idx = (size_t)(0.95 * (v.size()-1)); return v[idx]; }
    void Reset() { last.clear(); hist.clear(); }
    void SetPaused(bool p) { paused = p; }
    bool IsPaused() const { return paused; }
private:
    std::unordered_map<std::string,long long> last;
    std::unordered_map<std::string,std::vector<long long>> hist;
    bool paused = false;
};
}
