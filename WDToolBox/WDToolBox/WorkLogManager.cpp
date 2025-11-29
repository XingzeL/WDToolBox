// WorkLogManager.cpp: 工作日志管理器实现
//

#include "pch.h"
#include "WorkLogManager.h"
#include "WorkLogWriter.h"
#include <shlwapi.h>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

CWorkLogManager::CWorkLogManager()
{
}

CWorkLogManager::~CWorkLogManager()
{
	Clear();
}

bool CWorkLogManager::LoadFromConfig(const CString& strConfigPath)
{
	// 获取配置文件路径（如果未指定，使用程序目录下的 worklogs.ini）
	CString strIniPath = strConfigPath;
	if (strIniPath.IsEmpty())
	{
		TCHAR szModulePath[MAX_PATH];
		GetModuleFileName(NULL, szModulePath, MAX_PATH);
		CString strModulePath = szModulePath;
		int nPos = strModulePath.ReverseFind(_T('\\'));
		if (nPos >= 0)
		{
			strIniPath = strModulePath.Left(nPos + 1) + _T("worklogs.ini");
		}
		else
		{
			strIniPath = _T("worklogs.ini");
		}
	}

	// 检查配置文件是否存在
	if (!PathFileExists(strIniPath))
	{
		// 如果配置文件不存在，返回false
		return false;
	}

	// 读取UTF-8编码的文件内容
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

	// 将UTF-8转换为宽字符（Unicode）
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

	CString strContent;

	// 解析INI文件内容
#ifdef _UNICODE
	strContent = pWideBuffer;
#else
	// 多字节模式：将Unicode转换为多字节
	int nMultiByteLen = WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nWideLen, NULL, 0, NULL, NULL);
	if (nMultiByteLen > 0)
	{
		char* pMultiByteBuffer = new char[nMultiByteLen + 1];
		WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nWideLen, pMultiByteBuffer, nMultiByteLen, NULL, NULL);
		pMultiByteBuffer[nMultiByteLen] = 0;
		strContent = pMultiByteBuffer;
		delete[] pMultiByteBuffer;
	}
	else
	{
		strContent = _T("");
	}
#endif
	delete[] pWideBuffer;

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
		// 检查是否是库条目
		else if (!strCurrentCategory.IsEmpty())
		{
			if (!strLine.IsEmpty())
			{
				LogLibraryInfo library;
				library.strName = strLine;
				library.strCategory = strCurrentCategory;

				// 如果是新分类，记录到顺序列表中
				if (m_mapLibraries.find(strCurrentCategory) == m_mapLibraries.end())
				{
					m_vecCategoryOrder.push_back(strCurrentCategory);
				}

				m_mapLibraries[strCurrentCategory].push_back(library);
			}
		}
	}

	return true;
}

void CWorkLogManager::LoadDefaultLibraries()
{
	// 默认配置（作为后备）
	AddLibrary(_T("UI服务"), _T("库1"));
	AddLibrary(_T("UI服务"), _T("库2"));
	AddLibrary(_T("UI服务"), _T("库3"));

	AddLibrary(_T("用户工具"), _T("库1"));
	AddLibrary(_T("用户工具"), _T("库2"));
	AddLibrary(_T("用户工具"), _T("库3"));

	AddLibrary(_T("CSP库"), _T("库1"));
	AddLibrary(_T("CSP库"), _T("库2"));
	AddLibrary(_T("CSP库"), _T("库3"));

	AddLibrary(_T("维护"), _T("库1"));
	AddLibrary(_T("维护"), _T("库2"));
	AddLibrary(_T("维护"), _T("库3"));

	AddLibrary(_T("文档工具"), _T("库1"));
	AddLibrary(_T("文档工具"), _T("库2"));
	AddLibrary(_T("文档工具"), _T("库3"));
}

std::vector<LogLibraryInfo>& CWorkLogManager::GetLibrariesByCategory(const CString& strCategory)
{
	return m_mapLibraries[strCategory];
}

void CWorkLogManager::GetAllCategories(std::vector<CString>& categories)
{
	categories.clear();
	// 按照插入顺序返回分类
	for (size_t i = 0; i < m_vecCategoryOrder.size(); i++)
	{
		// 确保分类仍然存在（防止被清除后顺序列表未更新）
		if (m_mapLibraries.find(m_vecCategoryOrder[i]) != m_mapLibraries.end())
		{
			categories.push_back(m_vecCategoryOrder[i]);
		}
	}
}

void CWorkLogManager::Clear()
{
	m_mapLibraries.clear();
	m_vecCategoryOrder.clear();
}

// 辅助函数：添加日志库
void CWorkLogManager::AddLibrary(const CString& strCategory, const CString& strName)
{
	LogLibraryInfo library;
	library.strName = strName;
	library.strCategory = strCategory;

	// 如果是新分类，记录到顺序列表中
	if (m_mapLibraries.find(strCategory) == m_mapLibraries.end())
	{
		m_vecCategoryOrder.push_back(strCategory);
	}

	m_mapLibraries[strCategory].push_back(library);
}