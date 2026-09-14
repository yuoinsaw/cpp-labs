#pragma once

#include <stdexcept>
#include <string>

namespace core {

class AppException : public std::runtime_error {
public:
    explicit AppException(const std::string& message)
        : std::runtime_error(message) {}
};

class FileException : public AppException {
public:
    explicit FileException(const std::string& message)
        : AppException(message) {}
};

class FormatException : public AppException {
public:
    explicit FormatException(const std::string& message)
        : AppException(message) {}
};

}
