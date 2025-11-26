#pragma once
#include "CExecutor.h"

//CProcessLauncher: 根据传入的文件路径打开进程或者文件

class CProcessLauncher : CExecutor
{
public:
	CProcessLauncher() {};
	~CProcessLauncher() {};

public:
	virtual BOOL Execute(const CString& strContent) override
	{
		LaunchTool(strContent);
	}
	BOOL LaunchTool(const CString& strPath);
};

