// ToolManager.h: 工具管理器类
//

#pragma once

#include <vector>
#include <map>
#include "ConfigReader.h"
#include "Observer.h"

// 前向声明
class CExecutor;

// 工具信息结构
struct ToolInfo
{
	CString strName;      // 工具名称
	CString strPath;      // 工具路径
	CString strCategory;  // 工具分类
	HICON hIcon;         // 工具图标
	int nIconIndex;      // 图标索引（在图像列表中的索引）

	ToolInfo()
		: hIcon(NULL)
		, nIconIndex(-1)
	{
	}
};
// TODO: 需要重构，重构为模板基类，继承后实现具体功能
// 工具分类管理类（支持观察者模式）
class CToolManager : public CObservable
{
public:
	CToolManager(IConfigReader* pConfigReader = nullptr, CExecutor* pExecutor = nullptr);
	~CToolManager();

	// 添加工具
	// bNotify: 是否立即通知观察者（批量加载时设为 false，最后统一通知）
	void AddTool(const CString& strCategory, const CString& strName, const CString& strPath, bool bNotify = true);

	// 获取指定分类的所有工具
	std::vector<ToolInfo>& GetToolsByCategory(const CString& strCategory);

	// 获取所有分类名称
	void GetAllCategories(std::vector<CString>& categories);

	// 从配置文件加载工具
	bool LoadFromConfig(const CString& strConfigPath = _T(""));

	// 加载默认工具配置
	void LoadDefaultTools();

	// 加载工具图标
	void LoadToolIcons(CImageList& imageList);

	// 执行工具（委托给 Executor）
	BOOL ExecuteTool(const ToolInfo& tool);
	BOOL ExecuteTool(const CString& strPath);

	// 清理资源
	void Clear();

private:
	IConfigReader* m_pConfigReader;  // 配置读取器（依赖注入）
	bool m_bOwnConfigReader;         // 是否拥有 ConfigReader 的所有权

	CExecutor* m_pExecutor;          // 工具执行器（依赖注入）
	bool m_bOwnExecutor;             // 是否拥有 Executor 的所有权

	std::map<CString, std::vector<ToolInfo>> m_mapTools;  // 分类 -> 工具列表
	std::vector<CString> m_vecCategoryOrder;              // 分类顺序（保持插入顺序）

	// 获取默认配置文件路径
	CString GetDefaultConfigPath();
};

