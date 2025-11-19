#pragma once
#include <string>
#include "../Core/Logging/logging.h"

namespace Log {
    inline bool WriteLog(std::string ctx) { NOTURNAL_LOG_INFO("File", ctx); return true; }
    inline void Info(std::string ctx) { NOTURNAL_LOG_INFO("Core", ctx); }
    inline void Warning(std::string ctx, bool pause = false) { NOTURNAL_LOG_WARN("Core", ctx); }
    inline void Error(std::string ctx, bool fatal = true, bool pause = true) { if (fatal) NOTURNAL_LOG_FATAL("Core", ctx); else NOTURNAL_LOG_ERROR("Core", ctx); }
    inline void Fine(std::string ctx) { NOTURNAL_LOG_INFO("Core", ctx); }
    inline void Debug(std::string ctx, bool write = false) { NOTURNAL_LOG_DEBUG("Core", ctx); }
    inline void Custom(std::string ctx, int color) { NOTURNAL_LOG_INFO("Core", ctx); }
    inline void PreviousLine() {}
}
