//  Лабораторная работа №4
//
//  Условие: Разработать MFC-приложение, используя библиотеку классов STL.
//    1. Ядро приложения работает с использованием контейнеров и алгоритмов STL
//    2. Отсутствие C-конструкций в коде (использование вместо них конструкций C++)
//    3. Удобный интерфейс на базе MFC (QT, GDK...)
//    4. Система логирования, сохраняющая всю активность приложения и пользователя
//       в файл формата (время, важность, текст сообщения)
//    5. Обработка исключений, защита от дурака
//
//  Вариант 6: В файле записаны слова. Имеется таблица синонимов, записанная
//  в другом файле (для одного слова может быть несколько синонимов). Разработать
//  структуру данных, поддерживающую таблицу синонимов. Заменить каждое слово
//  в исходном файле на случайный синоним и результат записать в третий файл.

#include "pch.h"
#include "Lab4.h"
#include "Lab4Dlg.h"

BEGIN_MESSAGE_MAP(CLab4App, CWinApp)
END_MESSAGE_MAP()

CLab4App theApp;

CLab4App::CLab4App() {}

BOOL CLab4App::InitInstance() {
    CWinApp::InitInstance();

    INITCOMMONCONTROLSEX controls;
    controls.dwSize = sizeof(controls);
    controls.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&controls);

    AfxEnableControlContainer();
    SetRegistryKey(_T("Лабораторные работы"));

    CLab4Dlg dialog;
    m_pMainWnd = &dialog;
    dialog.DoModal();

    return FALSE;
}
