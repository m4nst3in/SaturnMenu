#pragma once
#include <string>
#include "../Core/Logging/logging.h"

namespace Log {
    inline bool WriteLog(std::string ctx) { SATURN_LOG_INFO("File", ctx); return true; }
    inline void Info(std::string ctx) { SATURN_LOG_INFO("Core", ctx); }
    inline void Warning(std::string ctx, bool pause = false) { SATURN_LOG_WARN("Core", ctx); }
    inline void Error(std::string ctx, bool fatal = true, bool pause = true) { if (fatal) SATURN_LOG_FATAL("Core", ctx); else SATURN_LOG_ERROR("Core", ctx); }
    inline void Fine(std::string ctx) { SATURN_LOG_INFO("Core", ctx); }
    inline void Debug(std::string ctx, bool write = false) { SATURN_LOG_DEBUG("Core", ctx); }
    inline void Custom(std::string ctx, int color) { SATURN_LOG_INFO("Core", ctx); }
    inline void PreviousLine() {}
}