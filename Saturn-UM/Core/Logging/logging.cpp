#include "logging.h"
#include <Windows.h>
#include "../../OS-ImGui/imgui/imgui.h"

namespace SaturnLogging {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

Logger::Logger() : running(false) {}

void Logger::add_sink(std::shared_ptr<ILogSink> s) {
    if (!s) return;
    sinks.push_back(s);
}

void Logger::log(Level lvl, const std::string& cat, const std::string& msg) {
    LogMessage m;
    m.time = std::chrono::system_clock::now();
    m.level = lvl;
    m.category = cat;
    m.text = msg;
    m.thread_id = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> lk(mtx);
        queue.push(m);
    }
    cv.notify_one();
}

void Logger::worker() {
    while (running.load()) {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&]{ return !running.load() || !queue.empty(); });
        while (!queue.empty()) {
            LogMessage m = queue.front();
            queue.pop();
            lk.unlock();
            for (auto& s : sinks) s->consume(m);
            lk.lock();
        }
    }
}

void Logger::start() {
    if (running.load()) return;
    running.store(true);
    th = std::thread([this]{ worker(); });
}

void Logger::stop() {
    if (!running.load()) return;
    running.store(false);
    cv.notify_all();
    if (th.joinable()) th.join();
}

std::string level_name(Level l) {
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
    FileSink();
    void consume(const LogMessage& m) override;
private:
    void rotate_if_needed();
    std::string path;
    size_t maxSize;
};

FileSink::FileSink() : path("Saturn.log"), maxSize(5 * 1024 * 1024) {}

void FileSink::rotate_if_needed() {
    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER size; GetFileSizeEx(h, &size); CloseHandle(h);
    if (size.QuadPart >= (LONGLONG)maxSize) {
        SYSTEMTIME st; GetLocalTime(&st);
        char suffix[64]; sprintf_s(suffix, "_%04d%02d%02d_%02d%02d%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        MoveFileA(path.c_str(), ("Saturn" + std::string(suffix)).c_str());
    }
}

void FileSink::consume(const LogMessage& m) {
    rotate_if_needed();
    FILE* f = nullptr; fopen_s(&f, path.c_str(), "ab"); if (!f) return;
    SYSTEMTIME st; GetLocalTime(&st);
    char buf[64]; sprintf_s(buf, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    std::string line = std::string(buf) + " [" + level_name(m.level) + "] [" + m.category + "] " + m.text + "\n";
    fwrite(line.c_str(), 1, line.size(), f);
    fclose(f);
}

class ImGuiSink : public ILogSink {
public:
    void consume(const LogMessage& m) override;
    void render_window();
    void set_open(bool v);
private:
    std::vector<LogMessage> buffer;
    bool open = false;
};

void ImGuiSink::consume(const LogMessage& m) {
    buffer.push_back(m);
    if (buffer.size() > 1000) buffer.erase(buffer.begin(), buffer.begin() + 200);
}

void ImGuiSink::set_open(bool v) { open = v; }

void ImGuiSink::render_window() {
    if (!open) return;
    if (!ImGui::GetCurrentContext()) return;
    ImGui::Begin("Saturn Logs", &open, ImGuiWindowFlags_NoCollapse);
    static int levelFilter = -1;
    static char search[128] = {0};
    ImGui::InputText("Search", search, sizeof(search));
    ImGui::SameLine();
    ImGui::Combo("Level", &levelFilter, "All\0Trace\0Debug\0Info\0Warn\0Error\0Fatal\0");
    ImGui::Separator();
    ImGui::BeginChild("LogList", ImVec2(0, 0), true);
    for (auto& m : buffer) {
        if (levelFilter >= 0 && (int)m.level != levelFilter) continue;
        if (search[0] != '\0') {
            if (m.text.find(search) == std::string::npos && m.category.find(search) == std::string::npos) continue;
        }
        ImGui::Text("[%s] [%s] %s", level_name(m.level).c_str(), m.category.c_str(), m.text.c_str());
    }
    ImGui::EndChild();
    ImGui::End();
}

static std::shared_ptr<ImGuiSink> g_imgui_sink;

std::shared_ptr<ILogSink> Logger::get_imgui_sink() { return imguiSink; }

void init_default() {
    auto& lg = Logger::instance();
    lg.add_sink(std::make_shared<ConsoleSink>());
    lg.add_sink(std::make_shared<FileSink>());
    g_imgui_sink = std::make_shared<ImGuiSink>();
    lg.add_sink(g_imgui_sink);
    lg.imguiSink = g_imgui_sink;
    lg.start();
}

void render_overlay() {
    if (g_imgui_sink) g_imgui_sink->render_window();
}

void set_overlay_open(bool v) {
    if (g_imgui_sink) g_imgui_sink->set_open(v);
}

}