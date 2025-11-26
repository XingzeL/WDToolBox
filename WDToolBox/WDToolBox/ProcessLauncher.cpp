#include "pch.h"
#include "ProcessLauncher.h"

// 启动工具
BOOL CProcessLauncher::LaunchTool(const CString& strPath)
{
	if (strPath.IsEmpty())
		return;

	HINSTANCE hResult = NULL;
	CString strPathLower = strPath;
	strPathLower.MakeLower();

	// 检查是否是URL（http://或https://开头）
	BOOL bIsURL = (strPathLower.Find(_T("http://")) == 0) || (strPathLower.Find(_T("https://")) == 0);

	// 检查是否是HTML文件
	CString strExt = strPathLower.Right(5);
	BOOL bIsHTML = (strExt == _T(".html")) || (strExt.Right(4) == _T(".htm"));

	// 特殊处理 URL 和 HTML 文件
	if (bIsURL || bIsHTML)
	{
		// 直接使用默认浏览器打开
		hResult = ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);
	}
	// 特殊处理 .cpl 文件（控制面板文件）
	else if (strPathLower.Right(4) == _T(".cpl"))
	{
		// 使用 control.exe 启动控制面板文件
		CString strControlPath = _T("control.exe");
		CString strParams;
		strParams.Format(_T("\"%s\""), strPath);

		// 检查 control.exe 是否存在（通常在 System32 目录）
		CString strSystem32 = _T("C:\\Windows\\System32\\control.exe");
		if (PathFileExists(strSystem32))
		{
			hResult = ShellExecute(NULL, _T("open"), strSystem32, strParams, NULL, SW_SHOWNORMAL);
		}
		else
		{
			// 如果找不到完整路径，尝试直接使用 control
			hResult = ShellExecute(NULL, _T("open"), strControlPath, strParams, NULL, SW_SHOWNORMAL);
		}
	}
	else
	{
		// 检查文件是否存在
		if (!PathFileExists(strPath))
		{
			return FALSE;
		}

		// 启动普通程序
		hResult = ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWNORMAL);
	}

	if ((INT_PTR)hResult <= 32)
	{
		return FALSE;
	}

	return TRUE;
}