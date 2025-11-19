// ToolInfo.cpp: 工具信息数据结构实现
//

#include "pch.h"
#include "ToolInfo.h"
#include <shlwapi.h>
#include <shellapi.h>
#include <shlobj.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

CToolManager::CToolManager()
{
}

CToolManager::~CToolManager()
{
	Clear();
}

void CToolManager::AddTool(const CString& strCategory, const CString& strName, const CString& strPath)
{
	ToolInfo tool;
	tool.strName = strName;
	tool.strPath = strPath;
	tool.strCategory = strCategory;

	// 如果是新分类，记录到顺序列表中
	if (m_mapTools.find(strCategory) == m_mapTools.end())
	{
		m_vecCategoryOrder.push_back(strCategory);
	}

	m_mapTools[strCategory].push_back(tool);
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
	imageList.Create(48, 48, ILC_COLOR32 | ILC_MASK, 0, 10);

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
}

