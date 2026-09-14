#include "TextProcessor.h"
#include "AppException.h"
#include "TextUtils.h"
#include "Logger.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <format>

namespace core {

std::wstring TextProcessor::readFile(const std::wstring& filePath) {
    std::wifstream file(filePath);
    if (!file.is_open())
        throw FileException("Не удалось открыть файл для чтения");

    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>()));

    std::wostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void TextProcessor::writeFile(const std::wstring& filePath, const std::wstring& text) {
    std::wofstream file(filePath, std::ios::out | std::ios::trunc);
    if (!file.is_open())
        throw FileException("Не удалось открыть файл для записи");

    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>()));
    file << text;

    if (!file.good())
        throw FileException("Ошибка при записи результата");
}

ProcessReport TextProcessor::process(const std::wstring& sourcePath,
                                     const std::wstring& targetPath,
                                     const SynonymTable& table,
                                     std::mt19937& generator)
{
    Logger& log = Logger::instance();

    if (toLower(sourcePath) == toLower(targetPath))
        throw AppException("Файл результата совпадает с исходным файлом");

    if (table.empty())
        throw AppException("Таблица синонимов пуста: сначала загрузите её");

    const std::wstring source = readFile(sourcePath);
    if (source.empty()) {
        log.warning(L"Исходный файл пуст");
        throw AppException("Исходный файл пуст: заменять нечего");
    }

    log.info(std::format(L"Прочитан исходный файл, символов: {}", source.size()));

    ProcessReport report;
    std::wstring result;
    result.reserve(source.size());

    std::wstring word;

    const auto flushWord = [&]() {
        if (word.empty()) return;

        report.totalWords++;

        if (table.contains(word)) {
            std::wstring synonym = table.randomSynonym(word, generator);

            if (isAllUpper(word))            synonym = toUpper(synonym);
            else if (isUpperChar(word[0]))   synonym = capitalize(synonym);

            result += synonym;
            report.replacedWords++;
            report.counts[toLower(word)]++;
            if (report.samples.size() < 30)
                report.samples.push_back(Replacement{ word, synonym });
        }
        else {
            result += word;
            report.missingWords++;
        }
        word.clear();
    };

    for (const wchar_t symbol : source) {
        if (isWordChar(symbol)) {
            word += symbol;
        }
        else {
            flushWord();
            result += symbol;
        }
    }
    flushWord();

    writeFile(targetPath, result);

    report.charCount = result.size();
    report.preview   = result.substr(0, std::min<std::size_t>(result.size(), 600));

    log.info(std::format(L"Записан результат: слов {}, заменено {}, без замены {}",
                         report.totalWords, report.replacedWords, report.missingWords));
    return report;
}

}
