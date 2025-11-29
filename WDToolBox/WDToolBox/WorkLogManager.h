#pragma once
#include "Executor.h"
class CWorkLogManager :
    public CExecutor
{
    void CWorkLogManger() {};

    virtual BOOL Execute(const CString& strContent);

};

