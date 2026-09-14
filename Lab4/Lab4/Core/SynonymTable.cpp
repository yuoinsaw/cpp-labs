#include "SynonymTable.h"
#include "AppException.h"
#include "TextUtils.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <codecvt>
#include <locale>
#include <format>

namespace core {

const std::vector<std::wstring> SynonymTable::emptyList_;

namespace {

bool parseLine(const std::wstring& rawLine,
               std::wstring& keyOut,
               std::vector<std::wstring>& synonymsOut)
{
    const std::wstring line = trim(rawLine);
    if (line.empty() || line.front() == L'#') return false;

    const std::size_t colon = line.find(L':');
    if (colon == std::wstring::npos) return false;

    keyOut = toLower(trim(line.substr(0, colon)));
    if (keyOut.empty()) return false;

    synonymsOut.clear();
    std::wistringstream stream(line.substr(colon + 1));
    std::wstring item;
    while (std::getline(stream, item, L',')) {
        const std::wstring word = trim(item);
        if (!word.empty()) synonymsOut.push_back(word);
    }
    return !synonymsOut.empty();
}

}

LoadReport SynonymTable::loadFromFile(const std::wstring& filePath) {
    std::wifstream file(filePath);
    if (!file.is_open())
        throw FileException("Не удалось открыть файл таблицы синонимов");

    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>()));

    table_.clear();
    LoadReport report;

    std::wstring line;
    while (std::getline(file, line)) {
        report.lineCount++;

        std::wstring key;
        std::vector<std::wstring> synonyms;

        if (!parseLine(line, key, synonyms)) {

            const std::wstring trimmed = trim(line);
            if (!trimmed.empty() && trimmed.front() != L'#') {
                report.skippedLines++;
                if (report.warnings.size() < 20)
                    report.warnings.push_back(
                        std::format(L"Строка {}: не удалось разобрать «{}»",
                                    report.lineCount, trimmed));
            }
            continue;
        }

        std::vector<std::wstring>& target = table_[key];
        for (const std::wstring& synonym : synonyms) {
            const bool alreadyThere = std::find(target.begin(), target.end(), synonym)
                                      != target.end();
            if (!alreadyThere) target.push_back(synonym);
        }
    }

    report.wordCount    = table_.size();
    report.synonymCount = synonymCount();

    if (table_.empty())
        throw FormatException("В файле не найдено ни одной корректной записи");

    return report;
}

bool SynonymTable::contains(const std::wstring& word) const {
    return table_.find(toLower(word)) != table_.end();
}

const std::vector<std::wstring>& SynonymTable::synonymsOf(const std::wstring& word) const {
    const auto it = table_.find(toLower(word));
    return (it == table_.end()) ? emptyList_ : it->second;
}

std::wstring SynonymTable::randomSynonym(const std::wstring& word,
                                         std::mt19937& generator) const
{
    const std::vector<std::wstring>& list = synonymsOf(word);
    if (list.empty()) return word;

    std::uniform_int_distribution<std::size_t> pick(0, list.size() - 1);
    return list[pick(generator)];
}

void SynonymTable::clear() {
    table_.clear();
}

std::size_t SynonymTable::synonymCount() const {
    return std::accumulate(table_.begin(), table_.end(), std::size_t{ 0 },
        [](std::size_t sum, const auto& pair) { return sum + pair.second.size(); });
}

}
