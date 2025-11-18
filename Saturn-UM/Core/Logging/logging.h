#pragma once
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <Windows.h>

// ImGui overlay support (optional include; guarded at use sites)
#include "../../OS-ImGui/imgui/imgui.h"

namespace SaturnLogging {

enum class Level { Trace, Debug, Info, Warn, Error, Fatal };

struct LogMessage {
    std::chrono::system_clock::time_point time;
    Level level;
    std::string category;
    std::string text;
    unsigned long thread_id;
};

struct ILogSink {
    virtual ~ILogSink() {}
    virtual void consume(const LogMessage&) = 0;
};

class Logger {
public:
    static Logger& instance() {
        static Logger inst; return inst;
    }
    void add_sink(std::shared_ptr<ILogSink> s) { if (s) sinks.push_back(s); }
    void log(Level lvl, const std::string& cat, const std::string& msg) {
        LogMessage m{ std::chrono::system_clock::now(), lvl, cat, msg, GetCurrentThreadId() };
        {
            std::lock_guard<std::mutex> lk(mtx);
            queue.push(m);
        }
        cv.notify_one();
    }
    void start() {
        if (running.load()) return; running.store(true);
        th = std::thread([this]{ worker(); });
    }
    void stop() {
        if (!running.load()) return; running.store(false); cv.notify_all(); if (th.joinable()) th.join();
    }
    std::shared_ptr<ILogSink> get_imgui_sink() { return imguiSink; }
    void set_imgui_sink(std::shared_ptr<ILogSink> s) { imguiSink = s; }
private:
    Logger() : running(false) {}
    void worker() {
        while (running.load()) {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [&]{ return !running.load() || !queue.empty(); });
            while (!queue.empty()) {
                LogMessage m = queue.front(); queue.pop(); lk.unlock();
                for (auto& s : sinks) s->consume(m);
                lk.lock();
            }
        }
    }
    std::vector<std::shared_ptr<ILogSink>> sinks;
    std::queue<LogMessage> queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread th;
    std::shared_ptr<ILogSink> imguiSink;
};

inline std::string level_name(Level l) {
    switch (l) {
        case Level::Trace: return "TRACE";
        case Level::Debug: return "DEBUG";
        case Level::Info:  return "INFO";
        case Level::Warn:  return "WARN";
        case Level::Error: return "ERROR";
        case Level::Fatal: return "FATAL";
    }
    return "INFO";
}

class ConsoleSink : public ILogSink {
public:
    void consume(const LogMessage& m) override {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        WORD color = 7;
        if (m.level == Level::Trace) color = 8;
        else if (m.level == Level::Debug) color = 11;
        else if (m.level == Level::Info) color = 15;
        else if (m.level == Level::Warn) color = 14;
        else if (m.level == Level::Error) color = 12;
        else if (m.level == Level::Fatal) color = 12;
        SetConsoleTextAttribute(h, color);
        SYSTEMTIME st; GetLocalTime(&st);
        char buf[64]; sprintf_s(buf, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        std::string line = std::string(buf) + " [" + level_name(m.level) + "] [" + m.category + "] " + m.text + "\n";
        DWORD written = 0; WriteConsoleA(h, line.c_str(), (DWORD)line.size(), &written, nullptr);
        SetConsoleTextAttribute(h, 7);
    }
};

class FileSink : public ILogSink {
public:
    FileSink() : path("Saturn.log"), maxSize(5 * 1024 * 1024) {}
    void consume(const LogMessage& m) override {
        // simple rotation by size
        HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER size; GetFileSizeEx(h, &size); CloseHandle(h);
            if (size.QuadPart >= (LONGLONG)maxSize) {
                SYSTEMTIME st; GetLocalTime(&st);
                char suffix[64]; sprintf_s(suffix, "_%04d%02d%02d_%02d%02d%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                MoveFileA(path.c_str(), ("Saturn" + std::string(suffix)).c_str());
            }
        }
        FILE* f = nullptr; fopen_s(&f, path.c_str(), "ab"); if (!f) return;
        SYSTEMTIME st; GetLocalTime(&st);
        char buf[64]; sprintf_s(buf, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        std::string line = std::string(buf) + " [" + level_name(m.level) + "] [" + m.category + "] " + m.text + "\n";
        fwrite(line.c_str(), 1, line.size(), f);
        fclose(f);
    }
private:
    std::string path; size_t maxSize;
};

class ImGuiSink : public ILogSink {
public:
    void consume(const LogMessage& m) override { buffer.push_back(m); if (buffer.size() > 1000) buffer.erase(buffer.begin(), buffer.begin() + 200); }
    void render_window() {
        if (!open) return; if (!ImGui::GetCurrentContext()) return;
        ImGui::Begin("Saturn Logs", &open, ImGuiWindowFlags_NoCollapse);
        static int levelFilter = -1; static char search[128] = {0};
        ImGui::InputText("Search", search, sizeof(search)); ImGui::SameLine();
        ImGui::Combo("Level", &levelFilter, "All\0Trace\0Debug\0Info\0Warn\0Error\0Fatal\0");
        ImGui::Separator(); ImGui::BeginChild("LogList", ImVec2(0, 0), true);
        for (auto& m : buffer) {
            if (levelFilter >= 0 && (int)m.level != levelFilter) continue;
            if (search[0] != '\0') { if (m.text.find(search) == std::string::npos && m.category.find(search) == std::string::npos) continue; }
            ImGui::Text("[%s] [%s] %s", level_name(m.level).c_str(), m.category.c_str(), m.text.c_str());
        }
        ImGui::EndChild(); ImGui::End();
    }
    void set_open(bool v) { open = v; }
private:
    std::vector<LogMessage> buffer; bool open = false;
};

inline std::shared_ptr<ImGuiSink> g_imgui_sink;
class PipeSink : public ILogSink {
public:
    PipeSink() {
        h = CreateFileA("\\\\.\\pipe\\SaturnLoaderLog", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    }
    void consume(const LogMessage& m) override {
        if (h == INVALID_HANDLE_VALUE) return;
        SYSTEMTIME st; GetLocalTime(&st);
        char buf[64]; sprintf_s(buf, "[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);
        std::string line = std::string(buf) + "[" + level_name(m.level) + "] [" + m.category + "] " + m.text + "\n";
        DWORD written = 0; WriteFile(h, line.c_str(), (DWORD)line.size(), &written, NULL);
    }
    ~PipeSink() { if (h != INVALID_HANDLE_VALUE) CloseHandle(h); }
private:
    HANDLE h = INVALID_HANDLE_VALUE;
};

inline void init_default() {
    auto& lg = Logger::instance();
    lg.add_sink(std::make_shared<ConsoleSink>());
    lg.add_sink(std::make_shared<FileSink>());
    g_imgui_sink = std::make_shared<ImGuiSink>();
    lg.add_sink(g_imgui_sink);
    lg.set_imgui_sink(g_imgui_sink);
    // Try pipe sink (best effort)
    lg.add_sink(std::make_shared<PipeSink>());
    lg.start();
}

inline void render_overlay() { if (g_imgui_sink) g_imgui_sink->render_window(); }
inline void set_overlay_open(bool v) { if (g_imgui_sink) g_imgui_sink->set_open(v); }

}

#define SATURN_LOG_TRACE(cat, msg) ::SaturnLogging::Logger::instance().log(::SaturnLogging::Level::Trace, (cat), (msg))
#define SATURN_LOG_DEBUG(cat, msg) ::SaturnLogging::Logger::instance().log(::SaturnLogging::Level::Debug, (cat), (msg))
#define SATURN_LOG_INFO(cat, msg)  ::SaturnLogging::Logger::instance().log(::SaturnLogging::Level::Info,  (cat), (msg))
#define SATURN_LOG_WARN(cat, msg)  ::SaturnLogging::Logger::instance().log(::SaturnLogging::Level::Warn,  (cat), (msg))
#define SATURN_LOG_ERROR(cat, msg) ::SaturnLogging::Logger::instance().log(::SaturnLogging::Level::Error, (cat), (msg))
#define SATURN_LOG_FATAL(cat, msg) ::SaturnLogging::Logger::instance().log(::SaturnLogging::Level::Fatal, (cat), (msg))