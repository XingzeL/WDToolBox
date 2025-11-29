// WorkLogManager.h: 工作日志管理器
//

#pragma once

#include <vector>
#include <map>

// 日志库信息结构
struct LogLibraryInfo
{
	CString strName;      // 库名称
	CString strCategory;  // 库分类
};

// 日志库管理类
class CWorkLogManager
{
public:
	CWorkLogManager();
	~CWorkLogManager();

	// 从配置文件加载日志库
	bool LoadFromConfig(const CString& strConfigPath = _T(""));

	// 加载默认日志库配置
	void LoadDefaultLibraries();

	// 获取指定分类的所有日志库
	std::vector<LogLibraryInfo>& GetLibrariesByCategory(const CString& strCategory);

	// 获取所有分类名称
	void GetAllCategories(std::vector<CString>& categories);

	// 清理资源
	void Clear();

	void AddLibrary(const CString& strCategory, const CString& strName);

private:
	std::map<CString, std::vector<LogLibraryInfo>> m_mapLibraries;  // 分类 -> 日志库列表
	std::vector<CString> m_vecCategoryOrder;      // 分类顺序（保持插入顺序）
};
