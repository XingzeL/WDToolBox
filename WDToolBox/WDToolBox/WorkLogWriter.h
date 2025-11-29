#pragma once
#include "Executor.h"
#include <vector>

struct WorkLogEntry
{
	CString content;
	time_t time;
};

class CWorkLogWriter :
    public CExecutor
{
public:
	CWorkLogWriter();
	virtual ~CWorkLogWriter();

	virtual BOOL Execute(const CString& strContent);

	void AddLog(const CString& strContent);
	void GetAllLogs(std::vector<WorkLogEntry>& logs);
	void SaveLogs();
	void LoadLogs();

private:
	std::vector<WorkLogEntry> m_logs;
};

