#include "pch.h"
#include "WorkLogWriter.h"

BOOL CWorkLogWriter::Execute(const CString& strContent)
{
	//进行日志的写入操作
	m_logs.push_back(strContent);

	FILE* pFile = NULL;
	errno_t err = fopen_s(&pFile, "logs.txt", "a");
	if (err != 0 || pFile == NULL)
	{
		return FALSE;
	}

#ifdef _UNICODE
	// Unicode模式：需要转换为多字节字符
	int nMultiByteLen = WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), NULL, 0, NULL, NULL);
	if (nMultiByteLen > 0)
	{
		char* pMultiByteBuffer = new char[nMultiByteLen + 1];
		WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), pMultiByteBuffer, nMultiByteLen, NULL, NULL);
		pMultiByteBuffer[nMultiByteLen] = 0;
		fprintf(pFile, "%s\n", pMultiByteBuffer);
		delete[] pMultiByteBuffer;
	}
#else
	// 多字节模式：使用GetString()获取const char*
	fprintf(pFile, "%s\n", strContent.GetString());
#endif

	fclose(pFile);
	return TRUE;
}

BOOL CWorkLogWriter::ExecuteH(const CString& strContent)
{
	//写入人类可读版本的日志到 logsh.txt
	FILE* pFile = NULL;
	errno_t err = fopen_s(&pFile, "logsh.txt", "a");
	if (err != 0 || pFile == NULL)
	{
		return FALSE;
	}

#ifdef _UNICODE
	// Unicode模式：需要转换为多字节字符
	int nMultiByteLen = WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), NULL, 0, NULL, NULL);
	if (nMultiByteLen > 0)
	{
		char* pMultiByteBuffer = new char[nMultiByteLen + 1];
		WideCharToMultiByte(CP_ACP, 0, strContent, strContent.GetLength(), pMultiByteBuffer, nMultiByteLen, NULL, NULL);
		pMultiByteBuffer[nMultiByteLen] = 0;
		fprintf(pFile, "%s\n", pMultiByteBuffer);
		delete[] pMultiByteBuffer;
	}
#else
	// 多字节模式：使用GetString()获取const char*
	fprintf(pFile, "%s\n", strContent.GetString());
#endif

	fclose(pFile);
	return TRUE;
}
