#pragma once
#include "Executor.h"
#include <vector>

struct WorkLogEntry
{
	CString content;
	time_t time;
}; //±£¡Ù

class CWorkLogWriter :
    public CExecutor
{
public:
	CWorkLogWriter() {};
	virtual ~CWorkLogWriter() {};

	virtual BOOL Execute(const CString& strContent);
private:
	std::vector<CString> m_logs;
};

