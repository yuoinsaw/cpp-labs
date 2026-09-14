#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace core {

enum class Severity {
    Info,
    Warning,
    Error
};

std::wstring toString(Severity level);

struct LogEntry {
    std::wstring time;
    Severity     level;
    std::wstring message;
};

class Logger {
public:

    static Logger& instance();

    void open(const std::wstring& filePath);
    void close();

    void write(Severity level, const std::wstring& message);

    void info(const std::wstring& message)    { write(Severity::Info,    message); }
    void warning(const std::wstring& message) { write(Severity::Warning, message); }
    void error(const std::wstring& message)   { write(Severity::Error,   message); }

    const std::vector<LogEntry>& entries() const { return entries_; }
    void clearEntries() { entries_.clear(); }

    static std::wstring timestamp();

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::wofstream        file_;
    std::vector<LogEntry> entries_;
};

}
