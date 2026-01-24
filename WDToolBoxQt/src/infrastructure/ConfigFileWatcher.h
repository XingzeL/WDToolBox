// ConfigFileWatcher.h: 配置文件监控类
//
#pragma once

#include <map>
#include <functional>
#include <QString>
#include <QObject>
#include <QFileSystemWatcher>

// 配置文件监控类
// 使用QFileSystemWatcher定期检查文件修改时间，当文件被修改时自动重新加载
class CConfigFileWatcher : public QObject
{
    Q_OBJECT

public:
    CConfigFileWatcher(QObject* parent = nullptr);
    ~CConfigFileWatcher();

    // 开始监控文件
    // strFilePath: 要监控的文件路径
    // callback: 文件变化时的回调函数（参数为文件路径）
    // 返回true表示成功，false表示失败
    bool StartWatch(const QString& strFilePath, std::function<void(const QString&)> callback);

    // 停止监控文件
    // strFilePath: 要停止监控的文件路径
    void StopWatch(const QString& strFilePath);

    // 停止所有监控
    void StopAll();

private slots:
    // 文件变化槽函数
    void onFileChanged(const QString& path);

private:
    // 监控信息结构
    struct WatchInfo
    {
        QString strFilePath;
        std::function<void(const QString&)> callback;
    };

    // 文件路径 -> 监控信息
    std::map<QString, WatchInfo> m_mapWatches;

    // Qt文件系统监控器
    QFileSystemWatcher* m_pWatcher;
};
