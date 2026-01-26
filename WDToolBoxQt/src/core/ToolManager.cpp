// ToolManager.cpp: 工具管理器实现
//
#pragma execution_character_set("utf-8")
#include "ToolManager.h"
#include "../infrastructure/Executor.h"
#include "../infrastructure/ProcessLauncher.h"
#include "../infrastructure/ConfigReader.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QListWidgetItem>
#include <QListWidget>
#include <QStyle>
#include <QApplication>
#include <QDir>
#include <QDebug>

CToolManager::CToolManager(IConfigReader* pConfigReader, CExecutor* pExecutor)
    : m_pConfigReader(pConfigReader)
    , m_bOwnConfigReader(false)
    , m_pExecutor(pExecutor)
    , m_bOwnExecutor(false)
{
    // 如果没有提供 ConfigReader，创建一个默认的
    if (m_pConfigReader == nullptr)
    {
        m_pConfigReader = new CIniConfigReader();
        m_bOwnConfigReader = true;
    }

    // 如果没有提供 Executor，创建一个默认的 ProcessLauncher
    if (m_pExecutor == nullptr)
    {
        m_pExecutor = new CProcessLauncher();
        m_bOwnExecutor = true;
    }
}

CToolManager::~CToolManager()
{
    Clear();

    // 如果拥有 ConfigReader 的所有权，释放它
    if (m_bOwnConfigReader && m_pConfigReader != nullptr)
    {
        delete m_pConfigReader;
        m_pConfigReader = nullptr;
    }

    // 如果拥有 Executor 的所有权，释放它
    if (m_bOwnExecutor && m_pExecutor != nullptr)
    {
        delete m_pExecutor;
        m_pExecutor = nullptr;
    }
}

void CToolManager::AddTool(const QString& strCategory, const QString& strName, const QString& strPath, bool bNotify)
{
    ToolInfo tool;
    tool.name = strName;
    tool.path = strPath;
    tool.category = strCategory;

    // 如果是新分类，记录到顺序列表中
    bool bNewCategory = (m_mapTools.find(strCategory) == m_mapTools.end());
    if (bNewCategory)
    {
        m_vecCategoryOrder.push_back(strCategory);
    }

    m_mapTools[strCategory].push_back(tool);
    qDebug() << "       strPath:" << strPath;
    // 只有在需要通知时才通知观察者（批量加载时设为 false，避免频繁通知）
    if (bNotify)
    {
        // 通知观察者：工具已添加
        NotifyObservers(QString("ToolAdded"), &tool);

        // 如果是新分类，通知分类变化
        if (bNewCategory)
        {
            NotifyObservers(QString("CategoryAdded"), (void*)strCategory.toUtf8().constData());
        }
    }
}

bool CToolManager::GetToolsByCategory(const QString& strCategory, std::vector<ToolInfo>& tools) const
{
    tools.clear();  // 清空传出参数

    auto it = m_mapTools.find(strCategory);
    if (it != m_mapTools.end())
    {
        qDebug() << "GetToolsByCategory - Found category: " << strCategory << ", tools count: " << it->second.size();
        tools = it->second;  // 复制数据到传出参数
        return true;  // 分类存在
    }
    else
    {
        qDebug() << "GetToolsByCategory - Category not found: " << strCategory;
        // tools 已经是空的，不需要操作
        return false;  // 分类不存在
    }
}

void CToolManager::GetAllCategories(std::vector<QString>& categories)
{
    categories.clear();
    // 按照插入顺序返回分类
    for (size_t i = 0; i < m_vecCategoryOrder.size(); i++)
    {
        // 确保分类仍然存在（防止被清除后顺序列表未更新）
        if (m_mapTools.find(m_vecCategoryOrder[i]) != m_mapTools.end())
        {
            categories.push_back(m_vecCategoryOrder[i]);
        }
    }
}

// Helper function to load icon for a single tool
static QIcon LoadIconForTool(const QString& strPath)
{
    QFileIconProvider provider;
    QIcon icon;
    QString strPathLower = strPath.toLower();

    // 检查是否是URL（http://或https://开头）
    bool bIsURL = strPathLower.startsWith("http://") || strPathLower.startsWith("https://");

    // 检查是否是HTML文件
    bool bIsHTML = strPathLower.endsWith(".html") || strPathLower.endsWith(".htm");

    // 特殊处理 URL 和 HTML 文件
    if (bIsURL || bIsHTML)
    {
        // 优先使用文件图标提供者的网络图标
        icon = provider.icon(QFileIconProvider::Network);

        // 如果网络图标不可用，尝试使用标准图标
        if (icon.isNull())
        {
            QApplication* pApp = qApp;
            if (pApp)
            {
                QStyle* pStyle = pApp->style();
                if (pStyle)
                {
                    // 使用网络驱动器图标（表示网络资源）
                    icon = pStyle->standardIcon(QStyle::SP_DriveNetIcon);
                }
            }
        }

        // 如果还是不可用，使用文件图标作为后备
        if (icon.isNull())
        {
            icon = provider.icon(QFileIconProvider::File);
        }
    }
    // 从文件提取图标（跨平台）
    else if (QFileInfo::exists(strPath))
    {
        QFileInfo fileInfo(strPath);
        icon = provider.icon(fileInfo);
    }
    else
    {
        // 文件不存在，尝试使用文件信息获取关联程序图标
        QFileInfo fileInfo(strPath);
        icon = provider.icon(fileInfo);
    }

    // 如果无法获取图标，使用默认图标
    if (icon.isNull())
    {
        icon = provider.icon(QFileIconProvider::File);
    }

    return icon;
}

void CToolManager::LoadAllToolIcons()
{
    // 只填充tool.icon，不创建列表项
    for (auto& categoryPair : m_mapTools)
    {
        for (auto& tool : categoryPair.second)
        {
            tool.icon = LoadIconForTool(tool.path);
        }
    }
}

void CToolManager::LoadToolIcons(QListWidget* pListWidget)
{
    if (!pListWidget)
        return;

    // 清空列表
    pListWidget->clear();

    int nIndex = 0;

    for (auto& categoryPair : m_mapTools)
    {
        for (auto& tool : categoryPair.second)
        {
            // 总是重新加载图标，确保应用最新的 element.ico 逻辑
            tool.icon = LoadIconForTool(tool.path);

            // 创建列表项并设置图标
            new QListWidgetItem(tool.icon, tool.name, pListWidget);
            tool.iconIndex = nIndex++;
        }
    }
}

QString CToolManager::GetDefaultConfigPath() const
{
    QString appPath = QCoreApplication::applicationDirPath();
    // 首先尝试在程序目录查找
    QString configPath = appPath + "/tools.ini";
    if (QFileInfo::exists(configPath))
    {
        return configPath;
    }
    // 如果不存在，尝试在config子目录查找（开发时）
    configPath = appPath + "/config/tools.ini";
    if (QFileInfo::exists(configPath))
    {
        return configPath;
    }
    // 如果都不存在，返回程序目录的路径（程序会创建）
    return appPath + "/tools.ini";
}

// 执行工具（委托给 Executor）
bool CToolManager::ExecuteTool(const ToolInfo& tool)
{
    if (m_pExecutor == nullptr)
        return false;
    return m_pExecutor->Execute(tool.path);
}

bool CToolManager::ExecuteTool(const QString& strPath)
{
    if (m_pExecutor == nullptr)
        return false;
    return m_pExecutor->Execute(strPath);
}

bool CToolManager::LoadFromConfig(const QString& strConfigPath)
{
    // 清空现有数据（但不通知观察者，避免UI闪烁）
    m_mapTools.clear();
    m_vecCategoryOrder.clear();

    // 获取配置文件路径（如果未指定，使用程序目录下的 tools.ini）
    QString strIniPath = strConfigPath;
    if (strIniPath.isEmpty())
    {
        strIniPath = GetDefaultConfigPath();
    }

    // 保存当前配置文件路径
    m_strConfigPath = strIniPath;

    // 使用 ConfigReader 加载配置
    if (m_pConfigReader == nullptr)
    {
        return false;
    }

    if (!m_pConfigReader->LoadFromFile(strIniPath))
    {
        return false;
    }

    // 获取所有节（分类）
    std::vector<QString> vecSections;
    if (!m_pConfigReader->GetSections(vecSections))
    {
        return false;
    }

    // 遍历每个分类，加载工具
    for (const QString& strCategory : vecSections)
    {
        // 获取该分类下的所有键（工具名称）
        std::vector<QString> vecKeys;
        qDebug() << "strCategory: " << strCategory;

        if (m_pConfigReader->GetKeys(strCategory, vecKeys))
        {
            // 遍历每个工具
            for (const QString& strToolName : vecKeys)
            {
                // 获取工具路径
                QString strToolPath = m_pConfigReader->GetValue(strCategory, strToolName);
                qDebug() << "   strToolName: " << strToolName;
                if (!strToolPath.isEmpty())
                {
                    // 去除引号
                    strToolPath = strToolPath.trimmed();
                    if (strToolPath.length() > 0 && strToolPath[0] == '"')
                    {
                        strToolPath = strToolPath.mid(1);
                    }
                    if (strToolPath.length() > 0 && strToolPath[strToolPath.length() - 1] == '"')
                    {
                        strToolPath = strToolPath.left(strToolPath.length() - 1);
                    }

                    // 添加工具（批量加载时不通知，最后统一通知）
                    AddTool(strCategory, strToolName, strToolPath, false);
                }
            }
        }
    }
    qDebug() << " ";
    // 通知观察者：配置已加载

    //2026.01.25在更新配置文件时，m_mapTools经过qDebug()所有的内容都是完整的，但是出现配置文件更新后这个函数插入内容没有插入完整的工具信息，而且没有图标
    //我觉得更新配置文件的时候只调用CToolManager::LoadFromConfig可能是不够的

    // 在通知观察者之前，先加载所有工具图标，确保UI更新时图标已准备好
    LoadAllToolIcons();

    // 验证数据完整性
    qDebug() << "LoadFromConfig - Before NotifyObservers, m_mapTools contents:";
    for (auto &it : m_mapTools)
    {
        qDebug() << "  Category: " << it.first << ", Tools count: " << it.second.size();
        for(auto &toolInfo : it.second)
        {
            qDebug() << "    Tool: " << toolInfo.name << ", path: " << toolInfo.path;
        }
    }
    qDebug() << "LoadFromConfig - Total categories: " << m_mapTools.size();

    NotifyObservers(QString("ConfigLoaded"), nullptr);

    // 通知后再次验证
    qDebug() << "LoadFromConfig - After NotifyObservers, m_mapTools contents:";
    for (auto &it : m_mapTools)
    {
        qDebug() << "  Category: " << it.first << ", Tools count: " << it.second.size();
    }
    qDebug() << " ";

    return true;
}

void CToolManager::LoadDefaultTools()
{
    // 默认配置（作为后备，批量加载时不通知，最后统一通知）
    // 源文件是UTF-8编码，直接使用QString构造函数
    // 根据平台加载不同的默认工具

#ifdef Q_OS_WIN
    // Windows 默认工具
    AddTool(QString("系统工具"), QString("任务管理器"), QString("C:\\Windows\\System32\\taskmgr.exe"), false);
    AddTool(QString("系统工具"), QString("注册表编辑器"), QString("C:\\Windows\\regedit.exe"), false);
    AddTool(QString("系统工具"), QString("系统配置"), QString("C:\\Windows\\System32\\msconfig.exe"), false);
    AddTool(QString("系统工具"), QString("设备管理器"), QString("C:\\Windows\\System32\\devmgmt.msc"), false);
    AddTool(QString("系统工具"), QString("服务"), QString("C:\\Windows\\System32\\services.msc"), false);
    AddTool(QString("系统工具"), QString("证书管理"), QString("C:\\Windows\\System32\\certmgr.msc"), false);

    AddTool(QString("磁盘工具"), QString("磁盘管理"), QString("C:\\Windows\\System32\\diskmgmt.msc"), false);
    AddTool(QString("磁盘工具"), QString("磁盘清理"), QString("C:\\Windows\\System32\\cleanmgr.exe"), false);

    AddTool(QString("网络工具"), QString("网络连接"), QString("C:\\Windows\\System32\\ncpa.cpl"), false);
    AddTool(QString("网络工具"), QString("命令提示符"), QString("C:\\Windows\\System32\\cmd.exe"), false);
#else
    // Linux 默认工具
    AddTool(QString("系统工具"), QString("系统监视器"), QString("gnome-system-monitor"), false);
    AddTool(QString("系统工具"), QString("系统设置"), QString("gnome-control-center"), false);
    AddTool(QString("系统工具"), QString("软件中心"), QString("software-center"), false);
    AddTool(QString("系统工具"), QString("磁盘使用分析"), QString("baobab"), false);
    AddTool(QString("系统工具"), QString("日志查看器"), QString("gnome-logs"), false);
    AddTool(QString("系统工具"), QString("文件管理器"), QString("nautilus"), false);

    AddTool(QString("磁盘工具"), QString("磁盘工具"), QString("gnome-disk-utility"), false);
    AddTool(QString("磁盘工具"), QString("分区编辑器"), QString("gparted"), false);

    AddTool(QString("网络工具"), QString("网络设置"), QString("nm-connection-editor"), false);
    AddTool(QString("网络工具"), QString("终端"), QString("gnome-terminal"), false);
    AddTool(QString("网络工具"), QString("浏览器"), QString("firefox"), false);
#endif

    // 批量加载完成后，统一通知一次
    NotifyObservers(QString("ConfigLoaded"), nullptr);
}

void CToolManager::Clear()
{
    m_mapTools.clear();
    m_vecCategoryOrder.clear();

    // 通知观察者：数据已清空
    NotifyObservers(QString("DataCleared"), nullptr);
}

bool CToolManager::SaveToConfig(const QString& strConfigPath)
{
    if (m_pConfigReader == nullptr)
        return false;

    // 获取配置文件路径（如果未指定，使用当前配置路径）
    QString strIniPath = strConfigPath;
    if (strIniPath.isEmpty())
    {
        strIniPath = m_strConfigPath;
        if (strIniPath.isEmpty())
        {
            strIniPath = GetDefaultConfigPath();
        }
    }

    // 先加载现有配置（如果文件存在），以保留其他配置项
    if (QFileInfo::exists(strIniPath))
    {
        m_pConfigReader->LoadFromFile(strIniPath);
    }

    // 将内存中的工具数据写入 ConfigReader
    for (const QString& strCategory : m_vecCategoryOrder)
    {
        auto it = m_mapTools.find(strCategory);
        if (it == m_mapTools.end())
            continue;

        for (const ToolInfo& tool : it->second)
        {
            m_pConfigReader->SetValue(strCategory, tool.name, tool.path);
        }
    }

    // 保存到文件
    bool bResult = m_pConfigReader->SaveToFile(strIniPath);
    if (bResult)
    {
        m_strConfigPath = strIniPath;
    }

    return bResult;
}
