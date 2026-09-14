#pragma once

#include "Resource.h"
#include "Core/SynonymTable.h"
#include "Core/TextProcessor.h"

#include <random>
#include <string>

class CLab4Dlg : public CDialog {
public:
    explicit CLab4Dlg(CWnd* parent = nullptr);

    enum { IDD = IDD_LAB4_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* exchange) override;
    virtual BOOL OnInitDialog() override;

    afx_msg void OnBnClickedBtnSource();
    afx_msg void OnBnClickedBtnSynonyms();
    afx_msg void OnBnClickedBtnResult();
    afx_msg void OnBnClickedBtnLoad();
    afx_msg void OnBnClickedBtnProcess();
    afx_msg void OnBnClickedBtnOpenResult();
    afx_msg void OnBnClickedBtnClearLog();

    DECLARE_MESSAGE_MAP()

private:

    void    setupLists();
    void    fillDefaultPaths();
    void    refreshTableView();
    void    refreshLogView();
    void    setStatus(const CString& text);
    void    reportError(const CString& stage, const std::exception& error);
    CString browseFile(bool openMode, const CString& title);

    CEdit     m_editSource;
    CEdit     m_editSynonyms;
    CEdit     m_editResult;
    CEdit     m_editPreview;
    CListCtrl m_listTable;
    CListCtrl m_listLog;

    core::SynonymTable  m_table;
    core::TextProcessor m_processor;
    std::mt19937        m_generator;
};
