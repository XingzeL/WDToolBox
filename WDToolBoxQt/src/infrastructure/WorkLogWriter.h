#pragma once
#include "Executor.h"
#include <QString>

class CWorkLogWriter : public CExecutor
{
public:
    CWorkLogWriter() {};
    virtual ~CWorkLogWriter() {};

    virtual bool Execute(const QString& strContent) override;
    bool ExecuteH(const QString& strContent);  // 写入人类可读版本到 logsh.txt
};
