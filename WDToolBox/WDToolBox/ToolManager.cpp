// ToolInfo.cpp: 工具信息数据结构实现
//

#include "pch.h"
#include "ToolManager.h"
#include "Executor.h"
#include "ProcessLauncher.h"
#include <shlwapi.h>
#include <shellapi.h>
#include <shlobj.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

CToolManager::CToolManager(IConfigReader* pConfigReader, CExecutor* pExecutor)
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

	// 如果没有提供 Executor，创建一个默认的 ProcessLauncher
	if (m_pExecutor == nullptr)
	{
		m_pExecutor = new CProcessLauncher();
		m_bOwnExecutor = true;
	}
}

CToolManager::~CToolManager()
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

void CToolManager::AddTool(const CString& strCategory, const CString& strName, const CString& strPath, bool bNotify)
{
	ToolInfo tool;
	tool.strName = strName;
	tool.strPath = strPath;
	tool.strCategory = strCategory;

	// 如果是新分类，记录到顺序列表中
	bool bNewCategory = (m_mapTools.find(strCategory) == m_mapTools.end());
	if (bNewCategory)
	{
		m_vecCategoryOrder.push_back(strCategory);
	}

	m_mapTools[strCategory].push_back(tool);

	// 只有在需要通知时才通知观察者（批量加载时设为 false，避免频繁通知）
	if (bNotify)
	{
		// 通知观察者：工具已添加
		NotifyObservers(_T("ToolAdded"), &tool);

		// 如果是新分类，通知分类变化
		if (bNewCategory)
		{
			NotifyObservers(_T("CategoryAdded"), (void*)(LPCTSTR)strCategory);
		}
	}
}

std::vector<ToolInfo>& CToolManager::GetToolsByCategory(const CString& strCategory)
{
	return m_mapTools[strCategory];
}

void CToolManager::GetAllCategories(std::vector<CString>& categories)
{
	categories.clear();
	// 按照插入顺序返回分类
	for (size_t i = 0; i < m_vecCategoryOrder.size(); i++)
	{
		// 确保分类仍然存在（防止被清除后顺序列表未更新）
		if (m_mapTools.find(m_vecCategoryOrder[i]) != m_mapTools.end())
		{
			categories.push_back(m_vecCategoryOrder[i]);
		}
	}
}

void CToolManager::LoadToolIcons(CImageList& imageList)
{
	// 清空图像列表
	imageList.DeleteImageList();

	// 创建图像列表（大图标）
	imageList.Create(48, 48, ILC_COLOR32, 0, 10);

	// 设置背景色与系统窗口背景一致，避免图标显示黑底
	imageList.SetBkColor(GetSysColor(COLOR_WINDOW));

	int nIndex = 0;
	for (auto& categoryPair : m_mapTools)
	{
		for (auto& tool : categoryPair.second)
		{
			HICON hIcon = NULL;

			// 从exe文件提取图标
			if (PathFileExists(tool.strPath))
			{
				SHFILEINFO sfi = { 0 };
				// 使用SHGetFileInfo提取大图标
				if (SHGetFileInfo(tool.strPath, 0, &sfi, sizeof(sfi),
					SHGFI_ICON | SHGFI_LARGEICON))
				{
					hIcon = sfi.hIcon;
				}

				// 如果提取失败，使用默认图标
				if (hIcon == NULL)
				{
					hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR);
				}
			}
			else
			{
				// 文件不存在，使用默认图标
				hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR);
			}

			if (hIcon != NULL)
			{
				imageList.Add(hIcon);
				tool.nIconIndex = nIndex++;
				tool.hIcon = hIcon;
			}
		}
	}
}

CString CToolManager::GetDefaultConfigPath()
{
	CString strIniPath;
	TCHAR szModulePath[MAX_PATH];
	GetModuleFileName(NULL, szModulePath, MAX_PATH);
	CString strModulePath = szModulePath;
	int nPos = strModulePath.ReverseFind(_T('\\'));
	if (nPos >= 0)
	{
		strIniPath = strModulePath.Left(nPos + 1) + _T("tools.ini");
	}
	else
	{
		strIniPath = _T("tools.ini");
	}
	return strIniPath;
}

// 执行工具（委托给 Executor）
BOOL CToolManager::ExecuteTool(const ToolInfo& tool)
{
	if (m_pExecutor == nullptr)
		return FALSE;
	return m_pExecutor->Execute(tool.strPath);
}

BOOL CToolManager::ExecuteTool(const CString& strPath)
{
	if (m_pExecutor == nullptr)
		return FALSE;
	return m_pExecutor->Execute(strPath);
}

bool CToolManager::LoadFromConfig(const CString& strConfigPath)
{
	// 清空现有数据
	Clear();

	// 获取配置文件路径（如果未指定，使用程序目录下的 tools.ini）
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

	// 遍历每个分类，加载工具
	for (const CString& strCategory : vecSections)
	{
		// 获取该分类下的所有键（工具名称）
		std::vector<CString> vecKeys;
		if (m_pConfigReader->GetKeys(strCategory, vecKeys))
		{
			// 遍历每个工具
			for (const CString& strToolName : vecKeys)
			{
				// 获取工具路径
				CString strToolPath = m_pConfigReader->GetValue(strCategory, strToolName);
				if (!strToolPath.IsEmpty())
				{
					// 去除引号
					strToolPath.Trim();
					if (strToolPath.GetLength() > 0 && strToolPath[0] == _T('"'))
					{
						strToolPath = strToolPath.Mid(1);
					}
					if (strToolPath.GetLength() > 0 && strToolPath[strToolPath.GetLength() - 1] == _T('"'))
					{
						strToolPath = strToolPath.Left(strToolPath.GetLength() - 1);
					}

					// 添加工具（批量加载时不通知，最后统一通知）
					AddTool(strCategory, strToolName, strToolPath, false);
				}
			}
		}
	}

	// 通知观察者：配置已加载
	NotifyObservers(_T("ConfigLoaded"), nullptr);

	return true;
}

void CToolManager::LoadDefaultTools()
{
	// 默认配置（作为后备，批量加载时不通知，最后统一通知）
	AddTool(CString(_T("系统工具")), CString(_T("任务管理器")), CString(_T("C:\\Windows\\System32\\taskmgr.exe")), false);
	AddTool(CString(_T("系统工具")), CString(_T("注册表编辑器")), CString(_T("C:\\Windows\\regedit.exe")), false);
	AddTool(CString(_T("系统工具")), CString(_T("系统配置")), CString(_T("C:\\Windows\\System32\\msconfig.exe")), false);
	AddTool(CString(_T("系统工具")), CString(_T("设备管理器")), CString(_T("C:\\Windows\\System32\\devmgmt.msc")), false);
	AddTool(CString(_T("系统工具")), CString(_T("服务")), CString(_T("C:\\Windows\\System32\\services.msc")), false);
	AddTool(CString(_T("系统工具")), CString(_T("证书管理")), CString(_T("C:\\Windows\\System32\\certmgr.msc")), false);

	AddTool(CString(_T("磁盘工具")), CString(_T("磁盘管理")), CString(_T("C:\\Windows\\System32\\diskmgmt.msc")), false);
	AddTool(CString(_T("磁盘工具")), CString(_T("磁盘清理")), CString(_T("C:\\Windows\\System32\\cleanmgr.exe")), false);

	AddTool(CString(_T("网络工具")), CString(_T("网络连接")), CString(_T("C:\\Windows\\System32\\ncpa.cpl")), false);
	AddTool(CString(_T("网络工具")), CString(_T("命令提示符")), CString(_T("C:\\Windows\\System32\\cmd.exe")), false);

	// 批量加载完成后，统一通知一次
	NotifyObservers(_T("ConfigLoaded"), nullptr);
}

void CToolManager::Clear()
{
	// 释放所有图标句柄
	for (auto& categoryPair : m_mapTools)
	{
		for (auto& tool : categoryPair.second)
		{
			if (tool.hIcon != NULL)
			{
				DestroyIcon(tool.hIcon);
				tool.hIcon = NULL;
			}
		}
	}
	m_mapTools.clear();
	m_vecCategoryOrder.clear();

	// 通知观察者：数据已清空
	NotifyObservers(_T("DataCleared"), nullptr);
}

