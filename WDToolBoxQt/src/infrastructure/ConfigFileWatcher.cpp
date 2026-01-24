// ConfigFileWatcher.cpp: 配置文件监控实现
//
#include "ConfigFileWatcher.h"

CConfigFileWatcher::CConfigFileWatcher(QObject* parent)
    : QObject(parent)
    , m_pWatcher(new QFileSystemWatcher(this))
{
    // 连接文件变化信号
    connect(m_pWatcher, &QFileSystemWatcher::fileChanged,
            this, &CConfigFileWatcher::onFileChanged);
}

CConfigFileWatcher::~CConfigFileWatcher()
{
    StopAll();
}

bool CConfigFileWatcher::StartWatch(const QString& strFilePath, std::function<void(const QString&)> callback)
{
    if (strFilePath.isEmpty() || !callback)
        return false;

    // 检查是否已在监控
    auto it = m_mapWatches.find(strFilePath);
    if (it != m_mapWatches.end())
    {
        // 已存在，更新回调函数
        it->second.callback = callback;
        return true;
    }

    // 添加文件到监控器
    if (!m_pWatcher->addPath(strFilePath))
    {
        return false;
    }

    // 保存监控信息
    WatchInfo info;
    info.strFilePath = strFilePath;
    info.callback = callback;

    m_mapWatches[strFilePath] = info;
    return true;
}

void CConfigFileWatcher::StopWatch(const QString& strFilePath)
{
    if (strFilePath.isEmpty())
        return;

    auto it = m_mapWatches.find(strFilePath);
    if (it != m_mapWatches.end())
    {
        // 从监控器移除文件
        m_pWatcher->removePath(strFilePath);

        // 从映射中移除
        m_mapWatches.erase(it);
    }
}

void CConfigFileWatcher::StopAll()
{
    // 移除所有监控的文件
    for (const auto& pair : m_mapWatches)
    {
        m_pWatcher->removePath(pair.first);
    }

    m_mapWatches.clear();
}

void CConfigFileWatcher::onFileChanged(const QString& path)
{
    // 查找对应的监控信息
    auto it = m_mapWatches.find(path);
    if (it != m_mapWatches.end())
    {
        // 调用回调函数
        if (it->second.callback)
        {
            it->second.callback(path);
        }

        // 注意：某些编辑器在保存文件时会先删除文件再创建，导致监控失效
        // 需要重新添加监控
        m_pWatcher->removePath(path);
        m_pWatcher->addPath(path);
    }
}
