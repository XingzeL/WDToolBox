// WorkLogManager.h: 工作日志管理器
//

#pragma once

#include <vector>
#include <map>
#include "ConfigReader.h"
#include "Observer.h"

// 前向声明
class CExecutor;

// 日志库信息结构
struct LogLibraryInfo
{
	CString strName;      // 库名称
	CString strCategory;  // 库分类
};

// 日志库管理类（支持观察者模式）
class CWorkLogManager : public CObservable
{
public:
	CWorkLogManager(IConfigReader* pConfigReader = nullptr, CExecutor* pExecutor = nullptr);
	~CWorkLogManager();

	// 从配置文件加载日志库
	bool LoadFromConfig(const CString& strConfigPath = _T(""));

	// 加载默认日志库配置
	void LoadDefaultLibraries();

	// 获取指定分类的所有日志库
	std::vector<LogLibraryInfo>& GetLibrariesByCategory(const CString& strCategory);

	// 获取所有分类名称
	void GetAllCategories(std::vector<CString>& categories);

	// 写入日志（委托给 Executor）
	BOOL WriteLog(const CString& strContent);

	// 清理资源
	void Clear();

	// 添加日志库
	// bNotify: 是否立即通知观察者（批量加载时设为 false，最后统一通知）
	void AddLibrary(const CString& strCategory, const CString& strName, bool bNotify = true);

	// 获取默认配置文件路径（供外部使用，如文件监控）
	CString GetDefaultConfigPath() const;

	// 获取当前使用的配置文件路径（供外部使用）
	CString GetConfigPath() const { return m_strConfigPath; }

private:
	IConfigReader* m_pConfigReader;  // 配置读取器（依赖注入）
	bool m_bOwnConfigReader;         // 是否拥有 ConfigReader 的所有权

	CExecutor* m_pExecutor;          // 日志写入器（依赖注入）
	bool m_bOwnExecutor;             // 是否拥有 Executor 的所有权

	std::map<CString, std::vector<LogLibraryInfo>> m_mapLibraries;  // 分类 -> 日志库列表
	std::vector<CString> m_vecCategoryOrder;      // 分类顺序（保持插入顺序）

private:
	CString m_strConfigPath;  // 当前使用的配置文件路径
};
