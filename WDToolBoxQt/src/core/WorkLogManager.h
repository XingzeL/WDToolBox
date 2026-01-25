// WorkLogManager.h: 工作日志管理器
//

#pragma once

#include <vector>
#include <map>
#include <QString>
#include "../infrastructure/ConfigReader.h"
#include "../infrastructure/Observer.h"
#include "LogLibraryInfo.h"

// 前向声明
class CExecutor;

// 日志库管理器类（支持观察者模式）
class CWorkLogManager : public CObservable
{
public:
    CWorkLogManager(IConfigReader* pConfigReader = nullptr, CExecutor* pExecutor = nullptr);
    ~CWorkLogManager();

    // 从配置文件加载日志库
    bool LoadFromConfig(const QString& strConfigPath = "");

    // 加载默认日志库配置
    void LoadDefaultLibraries();

    // 获取指定分类的所有日志库
    // 返回 true 表示分类存在，false 表示分类不存在
    // libraries 作为传出参数，如果分类不存在则为空
    bool GetLibrariesByCategory(const QString& strCategory, std::vector<LogLibraryInfo>& libraries) const;

    // 获取所有分类名称
    void GetAllCategories(std::vector<QString>& categories);

    // 写入日志（委托给 Executor）
    bool WriteLog(const QString& strContent);

    // 清理资源
    void Clear();

    // 添加日志库
    // bNotify: 是否立即通知观察者（批量加载时设为 false，最后统一通知）
    void AddLibrary(const QString& strCategory, const QString& strName, bool bNotify = true);

    // 获取默认配置文件路径（供外部使用，如文件监控）
    QString GetDefaultConfigPath() const;

    // 获取当前使用的配置文件路径（供外部使用）
    QString GetConfigPath() const { return m_strConfigPath; }

private:
    IConfigReader* m_pConfigReader;  // 配置读取器（依赖注入）
    bool m_bOwnConfigReader;         // 是否拥有 ConfigReader 的所有权

    CExecutor* m_pExecutor;          // 日志写入器（依赖注入）
    bool m_bOwnExecutor;             // 是否拥有 Executor 的所有权

    std::map<QString, std::vector<LogLibraryInfo>> m_mapLibraries;  // 分类 -> 日志库列表
    std::vector<QString> m_vecCategoryOrder;      // 分类顺序（保持插入顺序）

private:
    QString m_strConfigPath;  // 当前使用的配置文件路径
};
