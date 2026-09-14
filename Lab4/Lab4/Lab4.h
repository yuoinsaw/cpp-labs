#pragma once

#include "Resource.h"

class CLab4App : public CWinApp {
public:
    CLab4App();

    virtual BOOL InitInstance() override;

    DECLARE_MESSAGE_MAP()
};

extern CLab4App theApp;
