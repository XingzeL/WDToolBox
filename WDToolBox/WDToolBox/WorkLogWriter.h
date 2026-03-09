#pragma once
#include "Executor.h"
#include <vector>

struct WorkLogEntry
{
	CString content;
	time_t time;
}; //工作日志条目

class CWorkLogWriter : public CExecutor
{
public:
	CWorkLogWriter() {};
	virtual ~CWorkLogWriter() {};

	virtual BOOL Execute(const CString& strContent);
	BOOL ExecuteH(const CString& strContent); 
private:
	std::vector<CString> m_logs;
};

