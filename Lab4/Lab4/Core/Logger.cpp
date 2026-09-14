#include "Logger.h"
#include "AppException.h"

#include <chrono>
#include <format>
#include <codecvt>
#include <locale>

namespace core {

std::wstring toString(Severity level) {
    switch (level) {
        case Severity::Info:    return L"INFO";
        case Severity::Warning: return L"WARN";
        case Severity::Error:   return L"ERROR";
    }
    return L"?";
}

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

Logger::~Logger() {
    close();
}

std::wstring Logger::timestamp() {
    const auto now   = std::chrono::system_clock::now();
    const auto secs  = std::chrono::floor<std::chrono::seconds>(now);
    const auto local = std::chrono::zoned_time{ std::chrono::current_zone(), secs };
    return std::format(L"{:%Y-%m-%d %H:%M:%S}", local);
}

void Logger::open(const std::wstring& filePath) {
    close();

    file_.open(filePath, std::ios::out | std::ios::app);
    if (!file_.is_open())
        throw FileException("Не удалось открыть файл журнала");

    file_.imbue(std::locale(file_.getloc(),
        new std::codecvt_utf8<wchar_t>()));

    write(Severity::Info, L"=== Журнал открыт ===");
}

void Logger::close() {
    if (file_.is_open()) {
        write(Severity::Info, L"=== Журнал закрыт ===");
        file_.close();
    }
}

void Logger::write(Severity level, const std::wstring& message) {
    LogEntry entry{ timestamp(), level, message };
    entries_.push_back(entry);

    if (file_.is_open()) {
        file_ << entry.time << L" [" << toString(level) << L"] "
              << entry.message << L"\n";
        file_.flush();
    }
}

}
