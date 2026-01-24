#include "WorkLogWriter.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QCoreApplication>

bool CWorkLogWriter::Execute(const QString& strContent)
{
    // 进行日志的写入操作
    QString appPath = QCoreApplication::applicationDirPath();
    QString logPath = appPath + "/logs.txt";

    QFile file(logPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return false;
    }

    QTextStream stream(&file);
    // 使用UTF-8编码
    stream.setCodec("UTF-8");
    stream << strContent << "\n";
    file.close();

    return true;
}

bool CWorkLogWriter::ExecuteH(const QString& strContent)
{
    // 写入人类可读版本的日志到 logsh.txt
    QString appPath = QCoreApplication::applicationDirPath();
    QString logPath = appPath + "/logsh.txt";

    QFile file(logPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return false;
    }

    QTextStream stream(&file);
    // 使用UTF-8编码
    stream.setCodec("UTF-8");
    stream << strContent << "\n";
    file.close();

    return true;
}
