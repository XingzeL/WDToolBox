// WorkLogManager.cpp: 工作日志管理器实现
//

#include "pch.h"
#include "WorkLogManager.h"
#include "WorkLogWriter.h"
#include "Executor.h"
#include <windows.h>  // GetModuleFileName 需要

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

CString CWorkLogManager::GetDefaultConfigPath()
{
	CString strIniPath;
	TCHAR szModulePath[MAX_PATH];
	GetModuleFileName(NULL, szModulePath, MAX_PATH);
	CString strModulePath = szModulePath;
	int nPos = strModulePath.ReverseFind(_T('\\'));
	if (nPos >= 0)
	{
		strIniPath = strModulePath.Left(nPos + 1) + _T("worklogs.ini");
	}
	else
	{
		strIniPath = _T("worklogs.ini");
	}
	return strIniPath;
}

bool CWorkLogManager::LoadFromConfig(const CString& strConfigPath)
{
	// 清空现有数据
	Clear();

	// 获取配置文件路径（如果未指定，使用程序目录下的 worklogs.ini）
	CString strIniPath = strConfigPath;
	if (strIniPath.IsEmpty())
	{
		strIniPath = GetDefaultConfigPath();
	}

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
	std::vector<CString> vecSections;
	if (!m_pConfigReader->GetSections(vecSections))
	{
		return false;
	}

	// 遍历每个分类，加载日志库
	for (const CString& strCategory : vecSections)
	{
		// 获取该分类下的所有键（库名称）
		std::vector<CString> vecKeys;
		if (m_pConfigReader->GetKeys(strCategory, vecKeys))
		{
			// 遍历每个库
			for (const CString& strKey : vecKeys)
			{
				// 获取库名（值）
				CString strLibraryName = m_pConfigReader->GetValue(strCategory, strKey);
				// 如果值为空，使用键作为库名（兼容性处理）
				if (strLibraryName.IsEmpty())
				{
					strLibraryName = strKey;
				}

				if (!strLibraryName.IsEmpty())
				{
					// 添加日志库（批量加载时不通知，最后统一通知）
					AddLibrary(strCategory, strLibraryName, false);
				}
			}
		}
	}

	// 通知观察者：配置已加载
	NotifyObservers(_T("ConfigLoaded"), nullptr);

	return true;
}

void CWorkLogManager::LoadDefaultLibraries()
{
	// 默认配置（作为后备，批量加载时不通知，最后统一通知）
	AddLibrary(_T("UI服务"), _T("库1"), false);
	AddLibrary(_T("UI服务"), _T("库2"), false);
	AddLibrary(_T("UI服务"), _T("库3"), false);

	AddLibrary(_T("用户工具"), _T("库1"), false);
	AddLibrary(_T("用户工具"), _T("库2"), false);
	AddLibrary(_T("用户工具"), _T("库3"), false);

	AddLibrary(_T("CSP库"), _T("库1"), false);
	AddLibrary(_T("CSP库"), _T("库2"), false);
	AddLibrary(_T("CSP库"), _T("库3"), false);

	AddLibrary(_T("维护"), _T("库1"), false);
	AddLibrary(_T("维护"), _T("库2"), false);
	AddLibrary(_T("维护"), _T("库3"), false);

	AddLibrary(_T("文档工具"), _T("库1"), false);
	AddLibrary(_T("文档工具"), _T("库2"), false);
	AddLibrary(_T("文档工具"), _T("库3"), false);

	// 批量加载完成后，统一通知一次
	NotifyObservers(_T("ConfigLoaded"), nullptr);
}

std::vector<LogLibraryInfo>& CWorkLogManager::GetLibrariesByCategory(const CString& strCategory)
{
	return m_mapLibraries[strCategory];
}

void CWorkLogManager::GetAllCategories(std::vector<CString>& categories)
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
	m_mapLibraries.clear();
	m_vecCategoryOrder.clear();

	// 通知观察者：数据已清空
	NotifyObservers(_T("DataCleared"), nullptr);
}

// 写入日志（委托给 Executor）
BOOL CWorkLogManager::WriteLog(const CString& strContent)
{
	if (m_pExecutor == nullptr)
		return FALSE;
	return m_pExecutor->Execute(strContent);
}

// 辅助函数：添加日志库
void CWorkLogManager::AddLibrary(const CString& strCategory, const CString& strName, bool bNotify)
{
	LogLibraryInfo library;
	library.strName = strName;
	library.strCategory = strCategory;

	// 如果是新分类，记录到顺序列表中
	bool bNewCategory = (m_mapLibraries.find(strCategory) == m_mapLibraries.end());
	if (bNewCategory)
	{
		m_vecCategoryOrder.push_back(strCategory);
	}

	m_mapLibraries[strCategory].push_back(library);

	// 只有在需要通知时才通知观察者（批量加载时设为 false，避免频繁通知）
	if (bNotify)
	{
		// 通知观察者：库已添加
		NotifyObservers(_T("LibraryAdded"), &library);

		// 如果是新分类，通知分类变化
		if (bNewCategory)
		{
			NotifyObservers(_T("CategoryAdded"), (void*)(LPCTSTR)strCategory);
		}
	}
}
