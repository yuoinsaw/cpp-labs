#pragma once

#include <string>
#include <algorithm>
#include <codecvt>
#include <locale>

namespace core {

inline std::wstring fromUtf8(const std::string& text) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(text);
}

inline std::string toUtf8(const std::wstring& text) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(text);
}

inline wchar_t toLowerChar(wchar_t c) {
    if (c >= L'A'   && c <= L'Z')      return static_cast<wchar_t>(c + 32);
    if (c >= 0x0410 && c <= 0x042F)    return static_cast<wchar_t>(c + 32);
    if (c == 0x0401)                   return 0x0451;
    return c;
}

inline wchar_t toUpperChar(wchar_t c) {
    if (c >= L'a'   && c <= L'z')      return static_cast<wchar_t>(c - 32);
    if (c >= 0x0430 && c <= 0x044F)    return static_cast<wchar_t>(c - 32);
    if (c == 0x0451)                   return 0x0401;
    return c;
}

inline bool isUpperChar(wchar_t c) {
    return (c >= L'A' && c <= L'Z') || (c >= 0x0410 && c <= 0x042F) || c == 0x0401;
}

inline bool isWordChar(wchar_t c) {
    return (c >= L'A'   && c <= L'Z')
        || (c >= L'a'   && c <= L'z')
        || (c >= 0x0410 && c <= 0x044F)
        ||  c == 0x0401 || c == 0x0451
        ||  c == L'-';
}

inline std::wstring toLower(const std::wstring& s) {
    std::wstring result(s.size(), L' ');
    std::transform(s.begin(), s.end(), result.begin(), toLowerChar);
    return result;
}

inline std::wstring capitalize(const std::wstring& s) {
    if (s.empty()) return s;
    std::wstring result = s;
    result[0] = toUpperChar(result[0]);
    return result;
}

inline bool isAllUpper(const std::wstring& s) {
    if (s.size() < 2) return false;
    return std::all_of(s.begin(), s.end(),
        [](wchar_t c) { return !isWordChar(c) || isUpperChar(c); });
}

inline std::wstring toUpper(const std::wstring& s) {
    std::wstring result(s.size(), L' ');
    std::transform(s.begin(), s.end(), result.begin(), toUpperChar);
    return result;
}

inline std::wstring trim(const std::wstring& s) {
    const auto notSpace = [](wchar_t c) { return c != L' ' && c != L'\t' && c != L'\r'; };
    const auto first = std::find_if(s.begin(), s.end(), notSpace);
    if (first == s.end()) return std::wstring();
    const auto last = std::find_if(s.rbegin(), s.rend(), notSpace).base();
    return std::wstring(first, last);
}

}
