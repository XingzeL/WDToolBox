// ToolManager.h: 工具管理器类
//

#pragma once

#include <vector>
#include <map>

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

// 工具分类管理类
class CToolManager
{
public:
	CToolManager();
	~CToolManager();

	// 添加工具
	void AddTool(const CString& strCategory, const CString& strName, const CString& strPath);

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

	// 清理资源
	void Clear();

private:
	std::map<CString, std::vector<ToolInfo>> m_mapTools;  // 分类 -> 工具列表
	std::vector<CString> m_vecCategoryOrder;              // 分类顺序（保持插入顺序）
};

