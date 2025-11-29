// ToolInfo.cpp: 工具信息数据结构实现
//

#include "pch.h"
#include "ToolInfo.h"
#include <shlwapi.h>
#include <shellapi.h>
#include <shlobj.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

CToolManager::CToolManager()
{
}

CToolManager::~CToolManager()
{
	Clear();
}

void CToolManager::AddTool(const CString& strCategory, const CString& strName, const CString& strPath)
{
	ToolInfo tool;
	tool.strName = strName;
	tool.strPath = strPath;
	tool.strCategory = strCategory;

	// 如果是新分类，记录到顺序列表中
	if (m_mapTools.find(strCategory) == m_mapTools.end())
	{
		m_vecCategoryOrder.push_back(strCategory);
	}

	m_mapTools[strCategory].push_back(tool);
}

std::vector<ToolInfo>& CToolManager::GetToolsByCategory(const CString& strCategory)
{
	return m_mapTools[strCategory];
}

void CToolManager::GetAllCategories(std::vector<CString>& categories)
{
	categories.clear();
	// 按照插入顺序返回分类
	for (size_t i = 0; i < m_vecCategoryOrder.size(); i++)
	{
		// 确保分类仍然存在（防止被清除后顺序列表未更新）
		if (m_mapTools.find(m_vecCategoryOrder[i]) != m_mapTools.end())
		{
			categories.push_back(m_vecCategoryOrder[i]);
		}
	}
}

void CToolManager::LoadToolIcons(CImageList& imageList)
{
	// 清空图像列表
	imageList.DeleteImageList();

	// 创建图像列表（大图标）
	imageList.Create(48, 48, ILC_COLOR32, 0, 10);

	// 设置背景色与系统窗口背景一致，避免图标显示黑底
	imageList.SetBkColor(GetSysColor(COLOR_WINDOW));

	int nIndex = 0;
	for (auto& categoryPair : m_mapTools)
	{
		for (auto& tool : categoryPair.second)
		{
			HICON hIcon = NULL;
			
			// 从exe文件提取图标
			if (PathFileExists(tool.strPath))
			{
				SHFILEINFO sfi = { 0 };
				// 使用SHGetFileInfo提取大图标
				if (SHGetFileInfo(tool.strPath, 0, &sfi, sizeof(sfi), 
					SHGFI_ICON | SHGFI_LARGEICON))
				{
					hIcon = sfi.hIcon;
				}
				
				// 如果提取失败，使用默认图标
				if (hIcon == NULL)
				{
					hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR);
				}
			}
			else
			{
				// 文件不存在，使用默认图标
				hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR);
			}

			if (hIcon != NULL)
			{
				imageList.Add(hIcon);
				tool.nIconIndex = nIndex++;
				tool.hIcon = hIcon;
			}
		}
	}
}

bool CToolManager::LoadFromConfig(const CString& strConfigPath)
{
	// 获取配置文件路径（如果未指定，使用程序目录下的 tools.ini）
	CString strIniPath = strConfigPath;
	if (strIniPath.IsEmpty())
	{
		TCHAR szModulePath[MAX_PATH];
		GetModuleFileName(NULL, szModulePath, MAX_PATH);
		CString strModulePath = szModulePath;
		int nPos = strModulePath.ReverseFind(_T('\\'));
		if (nPos >= 0)
		{
			strIniPath = strModulePath.Left(nPos + 1) + _T("tools.ini");
		}
		else
		{
			strIniPath = _T("tools.ini");
		}
	}

	// 检查配置文件是否存在
	if (!PathFileExists(strIniPath))
	{
		// 如果配置文件不存在，返回false
		return false;
	}

	// 读取UTF-8编码的文件内容（支持Unicode和多字节字符集）
	HANDLE hFile = CreateFile(strIniPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// 获取文件大小
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0 || dwFileSize > 1024 * 1024) // 限制最大1MB
	{
		CloseHandle(hFile);
		return false;
	}

	// 读取文件内容
	char* pBuffer = new char[dwFileSize + 1];
	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, NULL))
	{
		delete[] pBuffer;
		CloseHandle(hFile);
		return false;
	}
	pBuffer[dwBytesRead] = 0;
	CloseHandle(hFile);

	// 检测并跳过BOM（UTF-8 BOM: EF BB BF）
	int nStartPos = 0;
	if (dwBytesRead >= 3 && (unsigned char)pBuffer[0] == 0xEF &&
		(unsigned char)pBuffer[1] == 0xBB && (unsigned char)pBuffer[2] == 0xBF)
	{
		nStartPos = 3;
	}

	// 将UTF-8转换为宽字符（Unicode）或多字节字符
#ifdef _UNICODE
	// Unicode模式：UTF-8 -> wchar_t
	int nWideLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos,
		dwBytesRead - nStartPos, NULL, 0);
	if (nWideLen <= 0)
	{
		delete[] pBuffer;
		return false;
	}

	wchar_t* pWideBuffer = new wchar_t[nWideLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, dwBytesRead - nStartPos,
		pWideBuffer, nWideLen);
	pWideBuffer[nWideLen] = 0;
	delete[] pBuffer;

	// 解析INI文件内容
	CString strContent = pWideBuffer;
	delete[] pWideBuffer;
#else
	// 多字节模式：UTF-8 -> 多字节字符（GBK）
	// 先转换为Unicode，再转换为多字节
	int nWideLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos,
		dwBytesRead - nStartPos, NULL, 0);
	if (nWideLen <= 0)
	{
		delete[] pBuffer;
		return false;
	}

	wchar_t* pWideBuffer = new wchar_t[nWideLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, dwBytesRead - nStartPos,
		pWideBuffer, nWideLen);
	pWideBuffer[nWideLen] = 0;
	delete[] pBuffer;

	// 将Unicode转换为多字节字符
	int nMultiByteLen = WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nWideLen, NULL, 0, NULL, NULL);
	if (nMultiByteLen <= 0)
	{
		delete[] pWideBuffer;
		return false;
	}

	char* pMultiByteBuffer = new char[nMultiByteLen + 1];
	WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nWideLen, pMultiByteBuffer, nMultiByteLen, NULL, NULL);
	pMultiByteBuffer[nMultiByteLen] = 0;
	delete[] pWideBuffer;

	// 解析INI文件内容
	CString strContent = pMultiByteBuffer;
	delete[] pMultiByteBuffer;
#endif

	CString strCurrentCategory;
	int nPos = 0;
	CString strLine;

	// 逐行解析
	while (nPos < strContent.GetLength())
	{
		int nLineEnd = strContent.Find(_T('\n'), nPos);
		if (nLineEnd == -1)
		{
			strLine = strContent.Mid(nPos);
			nPos = strContent.GetLength();
		}
		else
		{
			strLine = strContent.Mid(nPos, nLineEnd - nPos);
			nPos = nLineEnd + 1;
		}

		// 去除回车符和前后空格
		strLine.Trim();
		if (strLine.IsEmpty())
			continue;

		// 检查是否是分类行 [分类名]
		if (strLine.GetLength() > 2 && strLine[0] == _T('[') &&
			strLine[strLine.GetLength() - 1] == _T(']'))
		{
			strCurrentCategory = strLine.Mid(1, strLine.GetLength() - 2);
			strCurrentCategory.Trim();
		}
		// 检查是否是键值对 工具名称=工具路径
		else if (!strCurrentCategory.IsEmpty())
		{
			int nEqualPos = strLine.Find(_T('='));
			if (nEqualPos > 0)
			{
				CString strToolName = strLine.Left(nEqualPos);
				CString strToolPath = strLine.Mid(nEqualPos + 1);

				// 去除空格和引号
				strToolName.Trim();
				strToolPath.Trim();
				if (strToolPath.GetLength() > 0 && strToolPath[0] == _T('"'))
				{
					strToolPath = strToolPath.Mid(1);
				}
				if (strToolPath.GetLength() > 0 && strToolPath[strToolPath.GetLength() - 1] == _T('"'))
				{
					strToolPath = strToolPath.Left(strToolPath.GetLength() - 1);
				}

				// 添加工具
				if (!strToolName.IsEmpty() && !strToolPath.IsEmpty())
				{
					AddTool(strCurrentCategory, strToolName, strToolPath);
				}
			}
		}
	}

	return true;
}

void CToolManager::LoadDefaultTools()
{
	// 默认配置（作为后备）
	AddTool(CString(_T("系统工具")), CString(_T("任务管理器")), CString(_T("C:\\Windows\\System32\\taskmgr.exe")));
	AddTool(CString(_T("系统工具")), CString(_T("注册表编辑器")), CString(_T("C:\\Windows\\regedit.exe")));
	AddTool(CString(_T("系统工具")), CString(_T("系统配置")), CString(_T("C:\\Windows\\System32\\msconfig.exe")));
	AddTool(CString(_T("系统工具")), CString(_T("设备管理器")), CString(_T("C:\\Windows\\System32\\devmgmt.msc")));
	AddTool(CString(_T("系统工具")), CString(_T("服务")), CString(_T("C:\\Windows\\System32\\services.msc")));
	AddTool(CString(_T("系统工具")), CString(_T("证书管理")), CString(_T("C:\\Windows\\System32\\certmgr.msc")));

	AddTool(CString(_T("磁盘工具")), CString(_T("磁盘管理")), CString(_T("C:\\Windows\\System32\\diskmgmt.msc")));
	AddTool(CString(_T("磁盘工具")), CString(_T("磁盘清理")), CString(_T("C:\\Windows\\System32\\cleanmgr.exe")));

	AddTool(CString(_T("网络工具")), CString(_T("网络连接")), CString(_T("C:\\Windows\\System32\\ncpa.cpl")));
	AddTool(CString(_T("网络工具")), CString(_T("命令提示符")), CString(_T("C:\\Windows\\System32\\cmd.exe")));
}

void CToolManager::Clear()
{
	// 释放所有图标句柄
	for (auto& categoryPair : m_mapTools)
	{
		for (auto& tool : categoryPair.second)
		{
			if (tool.hIcon != NULL)
			{
				DestroyIcon(tool.hIcon);
				tool.hIcon = NULL;
			}
		}
	}
	m_mapTools.clear();
	m_vecCategoryOrder.clear();
}

