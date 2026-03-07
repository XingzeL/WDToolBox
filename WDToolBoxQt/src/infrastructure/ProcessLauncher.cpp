#include "ProcessLauncher.h"
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#ifdef Q_OS_WIN
#include <Windows.h>
#include <Shellapi.h>
#include <string>
#endif

// 启动工具
bool CProcessLauncher::LaunchTool(const QString& strPath)
{
    QString strTrimmed = strPath.trimmed();
    if (strTrimmed.isEmpty())
        return false;

    QString strPathLower = strTrimmed.toLower();

    // 检查是否是URL（http://或https://开头）
    bool bIsURL = strPathLower.startsWith("http://") || strPathLower.startsWith("https://");

    // URL：使用默认浏览器打开
    if (bIsURL)
    {
        return QDesktopServices::openUrl(QUrl(strTrimmed));
    }

    // 本地文件/文件夹：用系统默认关联打开（jpg、txt、pdf、文件夹等通用）
    QFileInfo fileInfo(strTrimmed);
    if (fileInfo.exists())
    {
        const QString strAbsPath = fileInfo.absoluteFilePath();
#ifdef Q_OS_WIN
        // Windows：用 ShellExecute("open") 通用打开，与 MFC 行为一致
        const std::wstring wpath = QDir::toNativeSeparators(strAbsPath).toStdWString();
        const HINSTANCE ret = ShellExecuteW(nullptr, L"open", wpath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        if (reinterpret_cast<INT_PTR>(ret) > 32)
            return true;
        // 失败时再尝试 Qt 方式
#endif
        if (QDesktopServices::openUrl(QUrl::fromLocalFile(strAbsPath)))
            return true;
    }

#ifdef Q_OS_WIN
    // Windows 特定处理
    // 特殊处理 .cpl 文件（控制面板文件）
    if (strPathLower.endsWith(".cpl"))
    {
        // 使用 control.exe 启动控制面板文件
        QString strControlPath = "control.exe";
        QStringList arguments;
        arguments << strTrimmed;

        // 检查 control.exe 是否存在（通常在 System32 目录）
        QString strSystem32 = QDir::toNativeSeparators("C:/Windows/System32/control.exe");
        if (QFileInfo::exists(strSystem32))
        {
            return QProcess::startDetached(strSystem32, arguments);
        }
        else
        {
            // 如果找不到完整路径，尝试直接使用 control
            return QProcess::startDetached(strControlPath, arguments);
        }
    }
    // 特殊处理 .msc 文件（Microsoft Management Console snap-in）
    else if (strPathLower.endsWith(".msc"))
    {
        // 使用 mmc.exe 启动 MMC 管理单元
        QString strMmcPath = "mmc.exe";
        QStringList arguments;
        arguments << strTrimmed;

        // 检查 mmc.exe 是否存在（通常在 System32 目录）
        QString strSystem32 = QDir::toNativeSeparators("C:/Windows/System32/mmc.exe");
        if (QFileInfo::exists(strSystem32))
        {
            return QProcess::startDetached(strSystem32, arguments);
        }
        else
        {
            // 如果找不到完整路径，尝试直接使用 mmc
            return QProcess::startDetached(strMmcPath, arguments);
        }
    }
#endif

    // 如果走到这里：
    // - 可能是“命令名/相对路径”（例如 notepad、cmd、python 等）
    // - 或者是不存在的绝对路径
    if (QFileInfo(strTrimmed).isAbsolute())
    {
        // 绝对路径但不存在：直接失败
        return false;
    }

    // 相对路径或命令名：让 QProcess 通过 PATH 查找并启动
    return QProcess::startDetached(strTrimmed);
}
