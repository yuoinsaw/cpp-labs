#include "pch.h"
#include "Lab4.h"
#include "Lab4Dlg.h"

#include "Core/Logger.h"
#include "Core/AppException.h"
#include "Core/TextUtils.h"

#include <filesystem>
#include <format>
#include <numeric>
#include <shellapi.h>

namespace fs = std::filesystem;
using core::Logger;
using core::Severity;

namespace {

fs::path executableFolder() {
    std::wstring buffer(MAX_PATH, L'\0');
    const DWORD length = ::GetModuleFileNameW(nullptr, buffer.data(),
                                              static_cast<DWORD>(buffer.size()));
    buffer.resize(length);
    return fs::path(buffer).parent_path();
}

fs::path findDataFolder() {
    fs::path current = executableFolder();
    for (int level = 0; level < 5; ++level) {
        const fs::path candidate = current / L"data";
        std::error_code code;
        if (fs::is_directory(candidate, code)) return candidate;
        if (!current.has_parent_path()) break;
        current = current.parent_path();
    }
    return fs::path();
}

std::wstring joinWords(const std::vector<std::wstring>& words) {
    if (words.empty()) return std::wstring();
    return std::accumulate(std::next(words.begin()), words.end(), words.front(),
        [](const std::wstring& acc, const std::wstring& item) {
            return acc + L", " + item;
        });
}

}

BEGIN_MESSAGE_MAP(CLab4Dlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_SOURCE,     &CLab4Dlg::OnBnClickedBtnSource)
    ON_BN_CLICKED(IDC_BTN_SYNONYMS,   &CLab4Dlg::OnBnClickedBtnSynonyms)
    ON_BN_CLICKED(IDC_BTN_RESULT,     &CLab4Dlg::OnBnClickedBtnResult)
    ON_BN_CLICKED(IDC_BTN_LOAD,       &CLab4Dlg::OnBnClickedBtnLoad)
    ON_BN_CLICKED(IDC_BTN_PROCESS,    &CLab4Dlg::OnBnClickedBtnProcess)
    ON_BN_CLICKED(IDC_BTN_OPENRESULT, &CLab4Dlg::OnBnClickedBtnOpenResult)
    ON_BN_CLICKED(IDC_BTN_CLEARLOG,   &CLab4Dlg::OnBnClickedBtnClearLog)
END_MESSAGE_MAP()

CLab4Dlg::CLab4Dlg(CWnd* parent)
    : CDialog(CLab4Dlg::IDD, parent)
{

    std::random_device device;
    m_generator.seed(device());
}

void CLab4Dlg::DoDataExchange(CDataExchange* exchange) {
    CDialog::DoDataExchange(exchange);
    DDX_Control(exchange, IDC_EDIT_SOURCE,   m_editSource);
    DDX_Control(exchange, IDC_EDIT_SYNONYMS, m_editSynonyms);
    DDX_Control(exchange, IDC_EDIT_RESULT,   m_editResult);
    DDX_Control(exchange, IDC_EDIT_PREVIEW,  m_editPreview);
    DDX_Control(exchange, IDC_LIST_TABLE,    m_listTable);
    DDX_Control(exchange, IDC_LIST_LOG,      m_listLog);
}

BOOL CLab4Dlg::OnInitDialog() {
    CDialog::OnInitDialog();

    setupLists();
    fillDefaultPaths();

    try {
        const fs::path logPath = executableFolder() / L"lab4_log.txt";
        Logger::instance().open(logPath.wstring());
        Logger::instance().info(L"Приложение запущено");
    }
    catch (const std::exception& error) {

        AfxMessageBox(CString(L"Журнал не ведётся: ")
                      + CString(core::fromUtf8(error.what()).c_str()),
                      MB_ICONWARNING);
    }

    refreshLogView();
    setStatus(L"Укажите файлы и нажмите «Загрузить таблицу синонимов».");
    return TRUE;
}

void CLab4Dlg::setupLists() {
    m_listTable.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listTable.InsertColumn(0, L"Слово",    LVCFMT_LEFT, 80);
    m_listTable.InsertColumn(1, L"Синонимы", LVCFMT_LEFT, 250);

    m_listLog.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listLog.InsertColumn(0, L"Время",      LVCFMT_LEFT, 130);
    m_listLog.InsertColumn(1, L"Важность",   LVCFMT_LEFT, 70);
    m_listLog.InsertColumn(2, L"Сообщение",  LVCFMT_LEFT, 520);
}

void CLab4Dlg::fillDefaultPaths() {
    const fs::path data = findDataFolder();
    if (data.empty()) return;

    std::error_code code;
    const fs::path source   = data / L"source.txt";
    const fs::path synonyms = data / L"synonyms.txt";

    if (fs::exists(source, code))   m_editSource.SetWindowText(source.c_str());
    if (fs::exists(synonyms, code)) m_editSynonyms.SetWindowText(synonyms.c_str());
    m_editResult.SetWindowText((data / L"result.txt").c_str());
}

void CLab4Dlg::setStatus(const CString& text) {
    SetDlgItemText(IDC_STATIC_STATS, text);
}

void CLab4Dlg::reportError(const CString& stage, const std::exception& error) {
    const CString message(core::fromUtf8(error.what()).c_str());
    Logger::instance().error(std::wstring(stage) + L": " + std::wstring(message));
    refreshLogView();
    setStatus(stage + L": " + message);
    AfxMessageBox(stage + L"\n\n" + message, MB_ICONERROR);
}

CString CLab4Dlg::browseFile(bool openMode, const CString& title) {
    CFileDialog dialog(openMode ? TRUE : FALSE,
                       L"txt", nullptr,
                       OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                       L"Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*||",
                       this);
    dialog.m_ofn.lpstrTitle = title;
    if (dialog.DoModal() != IDOK) return CString();
    return dialog.GetPathName();
}

void CLab4Dlg::OnBnClickedBtnSource() {
    const CString path = browseFile(true, L"Выберите исходный файл");
    if (!path.IsEmpty()) m_editSource.SetWindowText(path);
}

void CLab4Dlg::OnBnClickedBtnSynonyms() {
    const CString path = browseFile(true, L"Выберите файл таблицы синонимов");
    if (!path.IsEmpty()) m_editSynonyms.SetWindowText(path);
}

void CLab4Dlg::OnBnClickedBtnResult() {
    const CString path = browseFile(false, L"Куда сохранить результат");
    if (!path.IsEmpty()) m_editResult.SetWindowText(path);
}

void CLab4Dlg::OnBnClickedBtnLoad() {
    CString pathText;
    m_editSynonyms.GetWindowText(pathText);

    try {
        if (pathText.IsEmpty())
            throw core::AppException("Не указан файл таблицы синонимов");

        const std::wstring path(pathText);
        std::error_code code;
        if (!fs::exists(path, code))
            throw core::FileException("Указанный файл не существует");

        Logger::instance().info(L"Загрузка таблицы синонимов: " + path);

        const core::LoadReport report = m_table.loadFromFile(path);

        for (const std::wstring& warning : report.warnings)
            Logger::instance().warning(warning);

        Logger::instance().info(
            std::format(L"Таблица загружена: слов {}, синонимов {}, пропущено строк {}",
                        report.wordCount, report.synonymCount, report.skippedLines));

        refreshTableView();
        setStatus(CString(std::format(
            L"Таблица синонимов: {} слов, {} синонимов. Пропущено строк: {}.",
            report.wordCount, report.synonymCount, report.skippedLines).c_str()));
    }
    catch (const std::exception& error) {
        m_table.clear();
        refreshTableView();
        reportError(L"Загрузка таблицы синонимов", error);
        return;
    }
    refreshLogView();
}

void CLab4Dlg::OnBnClickedBtnProcess() {
    CString sourceText, resultText;
    m_editSource.GetWindowText(sourceText);
    m_editResult.GetWindowText(resultText);

    try {
        if (sourceText.IsEmpty())
            throw core::AppException("Не указан исходный файл");
        if (resultText.IsEmpty())
            throw core::AppException("Не указан файл результата");
        if (m_table.empty())
            throw core::AppException("Таблица синонимов не загружена");

        const std::wstring source(sourceText);
        const std::wstring target(resultText);

        std::error_code code;
        if (!fs::exists(source, code))
            throw core::FileException("Исходный файл не существует");

        Logger::instance().info(L"Начата обработка файла: " + source);

        const core::ProcessReport report =
            m_processor.process(source, target, m_table, m_generator);

        std::wstring preview = report.preview;
        for (std::size_t i = 0; i + 1 < preview.size(); ++i) {
            if (preview[i] == L'\n' && (i == 0 || preview[i - 1] != L'\r')) {
                preview.insert(i, 1, L'\r');
                ++i;
            }
        }
        m_editPreview.SetWindowText(preview.c_str());

        for (const core::Replacement& item : report.samples)
            Logger::instance().info(L"Замена: " + item.original + L" -> " + item.replacement);

        setStatus(CString(std::format(
            L"Готово. Слов: {}, заменено: {}, без синонимов: {}. Результат: {}",
            report.totalWords, report.replacedWords, report.missingWords,
            target).c_str()));
    }
    catch (const std::exception& error) {
        reportError(L"Обработка файла", error);
        return;
    }
    refreshLogView();
}

void CLab4Dlg::OnBnClickedBtnOpenResult() {
    CString pathText;
    m_editResult.GetWindowText(pathText);

    std::error_code code;
    if (pathText.IsEmpty() || !fs::exists(std::wstring(pathText), code)) {
        AfxMessageBox(L"Файла результата пока нет — сначала выполните замену.",
                      MB_ICONINFORMATION);
        return;
    }

    Logger::instance().info(L"Открытие файла результата во внешней программе");
    refreshLogView();
    ::ShellExecuteW(GetSafeHwnd(), L"open", pathText, nullptr, nullptr, SW_SHOWNORMAL);
}

void CLab4Dlg::OnBnClickedBtnClearLog() {
    Logger::instance().clearEntries();
    Logger::instance().info(L"Журнал в окне очищен (файл журнала сохранён)");
    refreshLogView();
}

void CLab4Dlg::refreshTableView() {
    m_listTable.DeleteAllItems();

    int row = 0;
    for (const auto& pair : m_table.data()) {
        m_listTable.InsertItem(row, pair.first.c_str());
        m_listTable.SetItemText(row, 1, joinWords(pair.second).c_str());
        ++row;
    }
}

void CLab4Dlg::refreshLogView() {
    m_listLog.DeleteAllItems();

    const std::vector<core::LogEntry>& entries = Logger::instance().entries();
    int row = 0;
    for (const core::LogEntry& entry : entries) {
        m_listLog.InsertItem(row, entry.time.c_str());
        m_listLog.SetItemText(row, 1, core::toString(entry.level).c_str());
        m_listLog.SetItemText(row, 2, entry.message.c_str());
        ++row;
    }
    if (row > 0) m_listLog.EnsureVisible(row - 1, FALSE);
}
