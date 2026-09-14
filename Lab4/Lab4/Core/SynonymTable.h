#pragma once

#include <string>
#include <vector>
#include <map>
#include <random>

namespace core {

struct LoadReport {
    std::size_t lineCount    = 0;
    std::size_t wordCount    = 0;
    std::size_t synonymCount = 0;
    std::size_t skippedLines = 0;
    std::vector<std::wstring> warnings;
};

class SynonymTable {
public:

    LoadReport loadFromFile(const std::wstring& filePath);

    bool contains(const std::wstring& word) const;

    const std::vector<std::wstring>& synonymsOf(const std::wstring& word) const;

    std::wstring randomSynonym(const std::wstring& word, std::mt19937& generator) const;

    void clear();
    bool empty() const { return table_.empty(); }

    std::size_t wordCount() const { return table_.size(); }
    std::size_t synonymCount() const;

    const std::map<std::wstring, std::vector<std::wstring>>& data() const { return table_; }

private:
    std::map<std::wstring, std::vector<std::wstring>> table_;
    static const std::vector<std::wstring> emptyList_;
};

}
