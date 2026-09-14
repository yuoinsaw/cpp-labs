#pragma once

#include "SynonymTable.h"

#include <string>
#include <vector>
#include <map>
#include <random>

namespace core {

struct Replacement {
    std::wstring original;
    std::wstring replacement;
};

struct ProcessReport {
    std::size_t totalWords    = 0;
    std::size_t replacedWords = 0;
    std::size_t missingWords  = 0;
    std::size_t charCount     = 0;
    std::vector<Replacement> samples;
    std::map<std::wstring, std::size_t> counts;
    std::wstring preview;
};

class TextProcessor {
public:

    ProcessReport process(const std::wstring& sourcePath,
                          const std::wstring& targetPath,
                          const SynonymTable& table,
                          std::mt19937& generator);

    static std::wstring readFile(const std::wstring& filePath);

private:
    static void writeFile(const std::wstring& filePath, const std::wstring& text);
};

}
