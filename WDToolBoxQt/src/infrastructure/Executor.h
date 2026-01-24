#pragma once
#include <QString>

class CExecutor
{
public:
    CExecutor() {};
    virtual ~CExecutor() {};

public:
    virtual bool Execute(const QString& strContent) = 0;
};
