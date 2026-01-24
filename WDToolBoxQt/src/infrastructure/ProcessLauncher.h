#pragma once
#include "Executor.h"

// CProcessLauncher: 根据传入的文件路径打开进程或者文件

class CProcessLauncher : public CExecutor
{
public:
    CProcessLauncher() {};
    ~CProcessLauncher() {};

public:
    virtual bool Execute(const QString& strContent) override
    {
        return LaunchTool(strContent);
    }

    bool LaunchTool(const QString& strPath);
};
