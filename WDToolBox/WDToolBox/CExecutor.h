#pragma once
class CExecutor
{
public:
	CExecutor(){};
	virtual ~CExecutor() {};

public:
	virtual BOOL Execute(const CString& strContent) = 0; //根据获取的内容执行
};

