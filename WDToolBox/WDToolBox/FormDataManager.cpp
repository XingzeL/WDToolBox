// FormDataManager.cpp: 实现文件
//

#include "pch.h"
#include "FormDataManager.h"
#include <shlwapi.h>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

CFormDataManager::CFormDataManager()
{
}

CFormDataManager::~CFormDataManager()
{
    Clear();
}

bool CFormDataManager::LoadFromFile(const CString& strFilePath)
{
    Clear();

    // 检查文件是否存在
    if (!PathFileExists(strFilePath))
    {
        return false;
    }

    // 读取文件内容（支持UTF-8）
    HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    // 获取文件大小
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == 0 || dwFileSize > 10 * 1024 * 1024) // 限制10MB
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

    CString strContent = pWideBuffer;
    delete[] pWideBuffer;
#else
    // 多字节模式：UTF-8 -> 多字节字符（GBK）
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

    CString strContent = pMultiByteBuffer;
    delete[] pMultiByteBuffer;
#endif

    // 解析CSV内容
    CString strCurrentFormName;
    FormData currentForm;
    bool bHasHeader = false;

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
        strLine.Replace(_T("\r"), _T(""));
        strLine.Trim();

        // 跳过空行
        if (strLine.IsEmpty())
            continue;

        // 判断是否为表单分隔符（例如：[表单名]）
        CString strFormName;
        if (IsFormSeparator(strLine, strFormName))
        {
            // 保存前一个表单
            if (!strCurrentFormName.IsEmpty() && !currentForm.vecHeaders.empty())
            {
                currentForm.strFormName = strCurrentFormName;
                m_mapForms[strCurrentFormName] = currentForm;
            }

            // 开始新表单
            strCurrentFormName = strFormName;
            currentForm = FormData();
            currentForm.strFormName = strFormName;
            bHasHeader = false;
        }
        else
        {
            // 解析CSV行
            std::vector<CString> vecFields = ParseCSVLine(strLine);

            if (!vecFields.empty())
            {
                if (!bHasHeader)
                {
                    // 第一行作为表头
                    currentForm.vecHeaders = vecFields;
                    bHasHeader = true;
                }
                else
                {
                    // 数据行
                    currentForm.vecRows.push_back(vecFields);
                }
            }
        }
    }

    // 保存最后一个表单
    if (!strCurrentFormName.IsEmpty() && !currentForm.vecHeaders.empty())
    {
        currentForm.strFormName = strCurrentFormName;
        m_mapForms[strCurrentFormName] = currentForm;
    }

    return !m_mapForms.empty();
}

void CFormDataManager::GetAllFormNames(std::vector<CString>& vecFormNames)
{
    vecFormNames.clear();
    for (auto it = m_mapForms.begin(); it != m_mapForms.end(); ++it)
    {
        vecFormNames.push_back(it->first);
    }
}

bool CFormDataManager::GetFormData(const CString& strFormName, FormData& formData)
{
    auto it = m_mapForms.find(strFormName);
    if (it != m_mapForms.end())
    {
        formData = it->second;
        return true;
    }
    return false;
}

void CFormDataManager::Clear()
{
    m_mapForms.clear();
}

std::vector<CString> CFormDataManager::ParseCSVLine(const CString& strLine)
{
    std::vector<CString> vecFields;
    CString strField;
    bool bInQuotes = false;

    for (int i = 0; i < strLine.GetLength(); ++i)
    {
        TCHAR ch = strLine[i];

        if (ch == _T('"'))
        {
            if (bInQuotes && i + 1 < strLine.GetLength() && strLine[i + 1] == _T('"'))
            {
                // 转义的双引号
                strField += _T('"');
                ++i; // 跳过下一个引号
            }
            else
            {
                // 切换引号状态
                bInQuotes = !bInQuotes;
            }
        }
        else if (ch == _T(',') && !bInQuotes)
        {
            // 字段分隔符
            vecFields.push_back(strField);
            strField.Empty();
        }
        else
        {
            strField += ch;
        }
    }

    // 添加最后一个字段
    vecFields.push_back(strField);

    return vecFields;
}

bool CFormDataManager::IsFormSeparator(const CString& strLine, CString& strFormName)
{
    // 判断是否为 [表单名] 格式
    if (strLine.GetLength() > 2 && strLine[0] == _T('[') &&
        strLine[strLine.GetLength() - 1] == _T(']'))
    {
        strFormName = strLine.Mid(1, strLine.GetLength() - 2);
        strFormName.Trim();
        return !strFormName.IsEmpty();
    }
    return false;
}
