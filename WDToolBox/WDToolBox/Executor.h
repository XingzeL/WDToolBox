#pragma once
class CExecutor
{
public:
	CExecutor(){};
	virtual ~CExecutor() {};

public:
	virtual BOOL Execute(const CString& strContent) = 0; 
};
