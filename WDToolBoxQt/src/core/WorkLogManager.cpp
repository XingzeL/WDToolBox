// WorkLogManager.cpp: 工作日志管理器实现
//
#pragma execution_character_set("utf-8")
#include "WorkLogManager.h"
#include "../infrastructure/WorkLogWriter.h"
#include "../infrastructure/Executor.h"
#include "../infrastructure/ConfigReader.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QDebug>

CWorkLogManager::CWorkLogManager(IConfigReader* pConfigReader, CExecutor* pExecutor)
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

    // 如果没有提供 Executor，创建一个默认的 WorkLogWriter
    if (m_pExecutor == nullptr)
    {
        m_pExecutor = new CWorkLogWriter();
        m_bOwnExecutor = true;
    }
}

CWorkLogManager::~CWorkLogManager()
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

QString CWorkLogManager::GetDefaultConfigPath() const
{
    QString appPath = QCoreApplication::applicationDirPath();
    // 首先尝试在程序目录查找
    QString configPath = appPath + "/worklogs.ini";
    if (QFileInfo::exists(configPath))
    {
        return configPath;
    }
    // 如果不存在，尝试在config子目录查找（开发时）
    configPath = appPath + "/config/worklogs.ini";
    if (QFileInfo::exists(configPath))
    {
        return configPath;
    }
    // 如果都不存在，返回程序目录的路径（程序会创建）
    return appPath + "/worklogs.ini";
}

bool CWorkLogManager::LoadFromConfig(const QString& strConfigPath)
{
    // 清空现有数据
    Clear();
    //m_mapLibraries.clear();
    //m_vecCategoryOrder.clear();

    // 获取配置文件路径（如果未指定，使用程序目录下的 worklogs.ini）
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

    // 遍历每个分类，加载日志库
    qDebug() << "CWorkLogManager::LoadFromConfig Before AddLibrary: ";
    for (QString& tmp : vecSections)
    {
        qDebug() << tmp;
    }

    qDebug() << "Inside For Loop to AddLibrary: ";
    for (QString& strCategory : vecSections)
    {
        // 获取该分类下的所有键（库名称）
        std::vector<QString> vecKeys;
        if (m_pConfigReader->GetKeys(strCategory, vecKeys))
        {
            qDebug() << "Iters " << strCategory; //According to the output, Iters all strings
            // 遍历每个库
            for (const QString& strKey : vecKeys)
            {
                // 获取库名（值）
                QString strLibraryName = m_pConfigReader->GetValue(strCategory, strKey);
                // 如果值为空，使用键作为库名（兼容性处理）
                if (strLibraryName.isEmpty())
                {
                    strLibraryName = strKey;
                }

                if (!strLibraryName.isEmpty())
                {
                    // 添加日志库（批量加载时不通知，最后统一通知）
                    AddLibrary(strCategory, strLibraryName, false);
                }
            }
        }
    }

    // 通知观察者：配置已加载
    NotifyObservers(QString("ConfigLoaded"), nullptr);

    return true;
}

void CWorkLogManager::LoadDefaultLibraries()
{
    // 默认配置（作为后备，批量加载时不通知，最后统一通知）
    // 源文件是UTF-8编码，直接使用QString构造函数
    AddLibrary(QString("UI服务"), QString("库1"), false);
    AddLibrary(QString("UI服务"), QString("库2"), false);
    AddLibrary(QString("UI服务"), QString("库3"), false);

    AddLibrary(QString("用户工具"), QString("库1"), false);
    AddLibrary(QString("用户工具"), QString("库2"), false);
    AddLibrary(QString("用户工具"), QString("库3"), false);

    AddLibrary(QString("CSP库"), QString("库1"), false);
    AddLibrary(QString("CSP库"), QString("库2"), false);
    AddLibrary(QString("CSP库"), QString("库3"), false);

    AddLibrary(QString("维护"), QString("库1"), false);
    AddLibrary(QString("维护"), QString("库2"), false);
    AddLibrary(QString("维护"), QString("库3"), false);

    AddLibrary(QString("文档工具"), QString("库1"), false);
    AddLibrary(QString("文档工具"), QString("库2"), false);
    AddLibrary(QString("文档工具"), QString("库3"), false);

    // 批量加载完成后，统一通知一次
    NotifyObservers(QString("ConfigLoaded"), nullptr);
}

bool CWorkLogManager::GetLibrariesByCategory(const QString& strCategory, std::vector<LogLibraryInfo>& libraries) const  //确保成员不被修改
{
    qDebug() << "GetLibrariesByCategory called with category: " << strCategory;

    libraries.clear();  // 清空传出参数

    auto it = m_mapLibraries.find(strCategory); // m_mapLibraries[strCategory] 在没有键的情况下会自动创建一个键。这个函数最后被定义为const后缀，同样防止这种情况出现
    if (it != m_mapLibraries.end())
    {
        qDebug() << "GetLibrariesByCategory - Found category: " << strCategory << ", libraries count: " << it->second.size();
        libraries = it->second;  // 复制数据到传出参数
        return true;  // 分类存在
    }
    else
    {
        qDebug() << "GetLibrariesByCategory - Category not found: " << strCategory;
        // libraries 已经是空的，不需要操作
        return false;
    }
}

void CWorkLogManager::GetAllCategories(std::vector<QString>& categories)
{
    categories.clear();
    // 按照插入顺序返回分类
    for (size_t i = 0; i < m_vecCategoryOrder.size(); i++)
    {
        // 确保分类仍然存在（防止被清除后顺序列表未更新）
        if (m_mapLibraries.find(m_vecCategoryOrder[i]) != m_mapLibraries.end())
        {
            categories.push_back(m_vecCategoryOrder[i]);
        }
    }
}

void CWorkLogManager::Clear()
{
    qDebug() << "CWorkLogManager::Clear(), clear m_mapLibraries";
    m_mapLibraries.clear();
    m_vecCategoryOrder.clear();

    // 通知观察者：数据已清空
    NotifyObservers(QString("DataCleared"), nullptr);
}

// 写入日志（委托给 Executor）
bool CWorkLogManager::WriteLog(const QString& strContent)
{
    if (m_pExecutor == nullptr)
        return false;
    return m_pExecutor->Execute(strContent);
}

// 辅助函数：添加日志库
void CWorkLogManager::AddLibrary(const QString& strCategory, const QString& strName, bool bNotify)
{
    LogLibraryInfo library;
    library.name = strName;
    library.category = strCategory;

    // 如果是新分类，记录到顺序列表中
    qDebug() << "CWorkLogManager::AddLibrary: ";
    QString temp = QString("");
    for (const auto& iter : m_mapLibraries)
    {
        temp += iter.first;
        temp += " ,";
    }
    qDebug() << temp;

    bool bNewCategory = (m_mapLibraries.find(strCategory) == m_mapLibraries.end()); //问题出在m_mapLibraries在刚开始的时候不是空的，而是有一个键
    if (bNewCategory)
    {
        qDebug() << "CWorkLogManager::AddLibrary() Add in map:" << strCategory;
        m_vecCategoryOrder.push_back(strCategory);
    }

    m_mapLibraries[strCategory].push_back(library);

    // 只有在需要通知时才通知观察者（批量加载时设为 false，避免频繁通知）
    if (bNotify)
    {
        // 通知观察者：库已添加
        NotifyObservers(QString("LibraryAdded"), &library);

        // 如果是新分类，通知分类变化
        if (bNewCategory)
        {
            NotifyObservers(QString("CategoryAdded"), (void*)strCategory.toUtf8().constData());
        }
    }
}
