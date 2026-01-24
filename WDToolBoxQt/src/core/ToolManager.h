// ToolManager.h: 工具管理器类
//

#pragma once

#include <vector>
#include <map>
#include <QString>
#include <QListWidget>
#include "ToolInfo.h"
#include "../infrastructure/ConfigReader.h"
#include "../infrastructure/Observer.h"

// 前向声明
class CExecutor;

// 工具分类管理器类（支持观察者模式）
class CToolManager : public CObservable
{
public:
    CToolManager(IConfigReader* pConfigReader = nullptr, CExecutor* pExecutor = nullptr);
    ~CToolManager();

    // 添加工具
    // bNotify: 是否立即通知观察者（批量加载时设为 false，最后统一通知）
    void AddTool(const QString& strCategory, const QString& strName, const QString& strPath, bool bNotify = true);

    // 获取指定分类的所有工具
    std::vector<ToolInfo>& GetToolsByCategory(const QString& strCategory);

    // 获取所有分类名称
    void GetAllCategories(std::vector<QString>& categories);

    // 从配置文件加载工具
    bool LoadFromConfig(const QString& strConfigPath = "");

    // 加载默认工具配置
    void LoadDefaultTools();

    // 加载工具图标
    void LoadToolIcons(QListWidget* pListWidget);

    // 执行工具（委托给 Executor）
    bool ExecuteTool(const ToolInfo& tool);
    bool ExecuteTool(const QString& strPath);

    // 清理资源
    void Clear();

    // 获取默认配置文件路径（供外部使用，如文件监控）
    QString GetDefaultConfigPath() const;

    // 获取当前使用的配置文件路径（供外部使用）
    QString GetConfigPath() const { return m_strConfigPath; }

private:
    IConfigReader* m_pConfigReader;  // 配置读取器（依赖注入）
    bool m_bOwnConfigReader;         // 是否拥有 ConfigReader 的所有权

    CExecutor* m_pExecutor;          // 工具执行器（依赖注入）
    bool m_bOwnExecutor;             // 是否拥有 Executor 的所有权

    std::map<QString, std::vector<ToolInfo>> m_mapTools;  // 分类 -> 工具列表
    std::vector<QString> m_vecCategoryOrder;              // 分类顺序（保持插入顺序）

private:
    QString m_strConfigPath;  // 当前使用的配置文件路径
};
