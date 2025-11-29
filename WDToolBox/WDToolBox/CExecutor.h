#pragma once
class CExecutor
{
public:
	CExecutor(){};
	virtual ~CExecutor() {};

public:
	virtual BOOL Execute(const CString& strContent) = 0; //根据获取的内容执行
};

void WriteHexToFile(const char* filename, const BYTE* buffer, DWORD len)
{
	FILE* fp = fopen(filename, "wb");

	for (int i = 0; i < len; ++i)
	{
		fprintf(fp, "%02X", buffer[i]);
		if ((i + 1) % 16 == 0)
		{
			fprintf(fp, "\n");
		}
		else
		{
			fprintf(fp, " ");
		}
	}

	if (len % 16 != 0)
	{
		fprintf(fp, "\n");
	}
	fclose(fp);
}