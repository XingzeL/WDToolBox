#include "ProcessLauncher.h"
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#ifdef Q_OS_WIN
#include <QDir>
#endif

// 启动工具
bool CProcessLauncher::LaunchTool(const QString& strPath)
{
    if (strPath.isEmpty())
        return false;

    QString strPathLower = strPath.toLower();

    // 检查是否是URL（http://或https://开头）
    bool bIsURL = strPathLower.startsWith("http://") || strPathLower.startsWith("https://");

    // 检查是否是HTML文件
    bool bIsHTML = strPathLower.endsWith(".html") || strPathLower.endsWith(".htm");

    // 特殊处理 URL 和 HTML 文件（跨平台）
    if (bIsURL || bIsHTML)
    {
        // 直接使用默认浏览器打开
        return QDesktopServices::openUrl(QUrl(strPath));
    }

#ifdef Q_OS_WIN
    // Windows 特定处理
    // 特殊处理 .cpl 文件（控制面板文件）
    if (strPathLower.endsWith(".cpl"))
    {
        // 使用 control.exe 启动控制面板文件
        QString strControlPath = "control.exe";
        QStringList arguments;
        arguments << strPath;

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
        arguments << strPath;

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

    // 通用处理：启动普通程序（跨平台）
    // 对于绝对路径，检查文件是否存在
    // 对于相对路径或命令名，让QProcess通过PATH查找
    if (QFileInfo(strPath).isAbsolute())
    {
        // 绝对路径：检查文件是否存在
        if (!QFileInfo::exists(strPath))
        {
            return false;
        }
    }
    // 相对路径或命令名：直接尝试启动（QProcess会通过PATH查找）

    // 启动程序（跨平台）
    return QProcess::startDetached(strPath);
}
