// ConfigReader.cpp: 统一配置读取接口实现
//
#include "pch.h"
#include "ConfigReader.h"
#include <shlwapi.h>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

CIniConfigReader::CIniConfigReader()
	: m_bLoaded(false)
{
}

CIniConfigReader::~CIniConfigReader()
{
	Clear();
}

bool CIniConfigReader::LoadFromFile(const CString& strFilePath)
{
	Clear();

	// 检查文件是否存在
	if (!PathFileExists(strFilePath))
	{
		return false;
	}

	// 读取UTF-8编码的配置文件内容
	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// 获取文件大小
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0 || dwFileSize > 1024 * 1024) // 限制1MB
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

	CString strContent(pWideBuffer);
	delete[] pWideBuffer;
#else
	// 多字节模式：UTF-8 -> char
	int nMultiLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos,
		dwBytesRead - nStartPos, NULL, 0);
	if (nMultiLen <= 0)
	{
		delete[] pBuffer;
		return false;
	}

	wchar_t* pWideBuffer = new wchar_t[nMultiLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, dwBytesRead - nStartPos,
		pWideBuffer, nMultiLen);
	pWideBuffer[nMultiLen] = 0;
	delete[] pBuffer;

	int nAnsiLen = WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nMultiLen, NULL, 0, NULL, NULL);
	char* pAnsiBuffer = new char[nAnsiLen + 1];
	WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nMultiLen, pAnsiBuffer, nAnsiLen, NULL, NULL);
	pAnsiBuffer[nAnsiLen] = 0;
	delete[] pWideBuffer;

	CString strContent(pAnsiBuffer);
	delete[] pAnsiBuffer;
#endif

	m_strConfigPath = strFilePath;
	m_bLoaded = ParseIniContent(strContent);
	return m_bLoaded;
}

bool CIniConfigReader::ParseIniContent(const CString& strContent)
{
	CString strCurrentSection;
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

		// 去除回车和前后空格
		strLine.Trim();
		if (strLine.IsEmpty())
			continue;

		// 判断是否是节 [节名]
		if (strLine.GetLength() > 2 && strLine[0] == _T('[') &&
			strLine[strLine.GetLength() - 1] == _T(']'))
		{
			strCurrentSection = strLine.Mid(1, strLine.GetLength() - 2);
			strCurrentSection.Trim();

			// 如果是新节，记录到顺序列表中
			if (m_mapConfig.find(strCurrentSection) == m_mapConfig.end())
			{
				m_vecSectionOrder.push_back(strCurrentSection);
			}
		}
		// 判断是否是键值对 键名=值
		else if (!strCurrentSection.IsEmpty())
		{
			int nEqualPos = strLine.Find(_T('='));
			if (nEqualPos > 0)
			{
				CString strKey = strLine.Left(nEqualPos);
				CString strValue = strLine.Mid(nEqualPos + 1);

				// 去除空格和引号
				strKey.Trim();
				strValue.Trim();
				if (strValue.GetLength() > 0 && strValue[0] == _T('"'))
				{
					strValue = strValue.Mid(1);
				}
				if (strValue.GetLength() > 0 && strValue[strValue.GetLength() - 1] == _T('"'))
				{
					strValue = strValue.Left(strValue.GetLength() - 1);
				}

				// 存储键值对
				if (!strKey.IsEmpty())
				{
					m_mapConfig[strCurrentSection][strKey] = strValue;
				}
			}
		}
	}

	return true;
}

bool CIniConfigReader::GetValue(const CString& strSection, const CString& strKey, CString& strValue)
{
	if (!m_bLoaded)
		return false;

	auto itSection = m_mapConfig.find(strSection);
	if (itSection == m_mapConfig.end())
		return false;

	auto itKey = itSection->second.find(strKey);
	if (itKey == itSection->second.end())
		return false;

	strValue = itKey->second;
	return true;
}

CString CIniConfigReader::GetValue(const CString& strSection, const CString& strKey, const CString& strDefault)
{
	CString strValue;
	if (GetValue(strSection, strKey, strValue))
		return strValue;
	return strDefault;
}

bool CIniConfigReader::GetKeys(const CString& strSection, std::vector<CString>& vecKeys)
{
	vecKeys.clear();
	if (!m_bLoaded)
		return false;

	auto itSection = m_mapConfig.find(strSection);
	if (itSection == m_mapConfig.end())
		return false;

	for (const auto& pair : itSection->second)
	{
		vecKeys.push_back(pair.first);
	}

	return true;
}

bool CIniConfigReader::GetSections(std::vector<CString>& vecSections)
{
	vecSections.clear();
	if (!m_bLoaded)
		return false;

	// 按照插入顺序返回节
	for (const auto& section : m_vecSectionOrder)
	{
		if (m_mapConfig.find(section) != m_mapConfig.end())
		{
			vecSections.push_back(section);
		}
	}

	return true;
}

void CIniConfigReader::Clear()
{
	m_bLoaded = false;
	m_strConfigPath.Empty();
	m_mapConfig.clear();
	m_vecSectionOrder.clear();
}

