#include "pch.h"
#include "WorkLogWriter.h"
#include <ctime>

CWorkLogWriter::CWorkLogWriter()
{
	LoadLogs();
}

CWorkLogWriter::~CWorkLogWriter()
{
	SaveLogs();
}

BOOL CWorkLogWriter::Execute(const CString& strContent)
{
	AddLog(strContent);
	return TRUE;
}

void CWorkLogWriter::AddLog(const CString& strContent)
{
	WorkLogEntry entry;
	entry.content = strContent;
	entry.time = time(NULL);
	m_logs.push_back(entry);
}

void CWorkLogWriter::GetAllLogs(std::vector<WorkLogEntry>& logs)
{
	logs = m_logs;
}

void CWorkLogWriter::SaveLogs()
{
	// TODO: 实现保存到文件的逻辑
	// 这里可以保存到INI文件、XML或数据库
}

void CWorkLogWriter::LoadLogs()
{
	// TODO: 实现从文件加载的逻辑
	// 这里可以从INI文件、XML或数据库加载
}
