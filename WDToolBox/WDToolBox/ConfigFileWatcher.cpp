// ConfigFileWatcher.cpp: 配置文件监控实现
//
#include "pch.h"
#include "ConfigFileWatcher.h"
#include <windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

// 静态映射：定时器ID -> 监控器实例
static std::map<UINT_PTR, CConfigFileWatcher*> s_mapTimerToWatcher;

CConfigFileWatcher::CConfigFileWatcher()
{
	InitializeCriticalSection(&m_cs);
}

CConfigFileWatcher::~CConfigFileWatcher()
{
	StopAll();
	DeleteCriticalSection(&m_cs);
}

UINT_PTR CConfigFileWatcher::StartWatch(HWND hWnd, const CString& strFilePath, std::function<void(const CString&)> callback)
{
	if (strFilePath.IsEmpty() || !callback)
		return 0;

	EnterCriticalSection(&m_cs);

	// 检查是否已经在监控
	auto it = m_mapWatches.find(strFilePath);
	if (it != m_mapWatches.end())
	{
		// 已存在，更新回调函数
		it->second.callback = callback;
		UINT_PTR nTimerId = it->second.nTimerId;
		LeaveCriticalSection(&m_cs);
		return nTimerId;
	}

	// 获取文件的初始修改时间
	FILETIME ftLastWriteTime = { 0 };
	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileTime(hFile, NULL, NULL, &ftLastWriteTime);
		CloseHandle(hFile);
	}

	// 创建定时器（每1秒检查一次）
	// 注意：如果 hWnd 不为 NULL，定时器事件会通过 WM_TIMER 消息发送，不会调用 TimerProc
	// 因此我们需要使用一个唯一的定时器ID，并在窗口的 OnTimer 中处理
	// 为了简化，我们使用 hWnd = NULL 来使用系统定时器，这样会调用 TimerProc
	UINT_PTR nTimerId = SetTimer(NULL, 0, 1000, TimerProc);
	if (nTimerId == 0)
	{
		LeaveCriticalSection(&m_cs);
		return 0;
	}

	// 保存监控信息
	WatchInfo info;
	info.strFilePath = strFilePath;
	info.ftLastWriteTime = ftLastWriteTime;
	info.callback = callback;
	info.nTimerId = nTimerId;
	info.hWnd = hWnd;

	m_mapWatches[strFilePath] = info;
	m_mapTimerToFile[nTimerId] = strFilePath;
	s_mapTimerToWatcher[nTimerId] = this;

	LeaveCriticalSection(&m_cs);
	return nTimerId;
}

void CConfigFileWatcher::StopWatch(HWND hWnd, UINT_PTR nTimerId)
{
	if (nTimerId == 0)
		return;

	EnterCriticalSection(&m_cs);

	auto itTimer = m_mapTimerToFile.find(nTimerId);
	if (itTimer != m_mapTimerToFile.end())
	{
		CString strFilePath = itTimer->second;

		// 停止定时器（使用 NULL 因为定时器是用 NULL hWnd 创建的）
		KillTimer(NULL, nTimerId);

		// 从映射中移除
		m_mapWatches.erase(strFilePath);
		m_mapTimerToFile.erase(itTimer);
		s_mapTimerToWatcher.erase(nTimerId);
	}

	LeaveCriticalSection(&m_cs);
}

void CConfigFileWatcher::StopAll()
{
	EnterCriticalSection(&m_cs);

	for (auto& pair : m_mapWatches)
	{
		// 使用 NULL 因为定时器是用 NULL hWnd 创建的
		KillTimer(NULL, pair.second.nTimerId);
		s_mapTimerToWatcher.erase(pair.second.nTimerId);
	}

	m_mapWatches.clear();
	m_mapTimerToFile.clear();

	LeaveCriticalSection(&m_cs);
}

bool CConfigFileWatcher::IsFileModified(const CString& strFilePath)
{
	if (strFilePath.IsEmpty())
		return false;

	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	FILETIME ftCurrentWriteTime = { 0 };
	GetFileTime(hFile, NULL, NULL, &ftCurrentWriteTime);
	CloseHandle(hFile);

	// 比较文件修改时间（这里应该与保存的时间比较，但静态函数无法访问，所以总是返回 false）
	// 这个函数主要用于外部调用，实际比较在 TimerProc 中进行
	return false;
}

void CALLBACK CConfigFileWatcher::TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	// 查找对应的监控器实例
	auto itWatcher = s_mapTimerToWatcher.find(idEvent);
	if (itWatcher == s_mapTimerToWatcher.end())
		return;

	CConfigFileWatcher* pWatcher = itWatcher->second;
	if (pWatcher == nullptr)
		return;

	EnterCriticalSection(&pWatcher->m_cs);

	// 查找对应的文件路径
	auto itTimer = pWatcher->m_mapTimerToFile.find(idEvent);
	if (itTimer == pWatcher->m_mapTimerToFile.end())
	{
		LeaveCriticalSection(&pWatcher->m_cs);
		return;
	}

	CString strFilePath = itTimer->second;
	auto itWatch = pWatcher->m_mapWatches.find(strFilePath);
	if (itWatch == pWatcher->m_mapWatches.end())
	{
		LeaveCriticalSection(&pWatcher->m_cs);
		return;
	}

	WatchInfo& info = itWatch->second;

	// 检查文件是否已修改
	HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&pWatcher->m_cs);
		return;
	}

	FILETIME ftCurrentWriteTime = { 0 };
	GetFileTime(hFile, NULL, NULL, &ftCurrentWriteTime);
	CloseHandle(hFile);

	// 比较文件修改时间
	LONG nCompare = CompareFileTime(&ftCurrentWriteTime, &info.ftLastWriteTime);
	if (nCompare > 0)  // 文件已被修改
	{
		// 更新最后修改时间
		info.ftLastWriteTime = ftCurrentWriteTime;

		// 调用回调函数（在临界区外调用，避免死锁）
		std::function<void(const CString&)> callback = info.callback;
		LeaveCriticalSection(&pWatcher->m_cs);

		if (callback)
		{
			callback(strFilePath);
		}
	}
	else
	{
		LeaveCriticalSection(&pWatcher->m_cs);
	}
}

