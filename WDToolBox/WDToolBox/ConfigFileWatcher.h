// ConfigFileWatcher.h: 配置文件监控类
//
#pragma once

#include <map>
#include <functional>

// 配置文件监控类
// 使用定时器定期检查文件修改时间，当文件被修改时自动重新加载
class CConfigFileWatcher
{
public:
	CConfigFileWatcher();
	~CConfigFileWatcher();

	// 开始监控文件
	// hWnd: 窗口句柄（用于定时器，可以为 NULL 使用系统定时器）
	// strFilePath: 要监控的文件路径
	// callback: 文件变化时的回调函数（参数为文件路径）
	// 返回定时器ID，失败返回0
	UINT_PTR StartWatch(HWND hWnd, const CString& strFilePath, std::function<void(const CString&)> callback);

	// 停止监控文件
	// hWnd: 窗口句柄（与 StartWatch 时相同）
	// nTimerId: StartWatch 返回的定时器ID
	void StopWatch(HWND hWnd, UINT_PTR nTimerId);

	// 停止所有监控
	void StopAll();

	// 检查文件是否已修改（供定时器回调使用）
	// strFilePath: 文件路径
	// 返回 true 表示文件已修改
	static bool IsFileModified(const CString& strFilePath);

private:
	// 定时器回调函数
	static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	// 监控信息结构
	struct WatchInfo
	{
		CString strFilePath;
		FILETIME ftLastWriteTime;
		std::function<void(const CString&)> callback;
		UINT_PTR nTimerId;
		HWND hWnd;
	};

	// 文件路径 -> 监控信息
	std::map<CString, WatchInfo> m_mapWatches;

	// 定时器ID -> 文件路径
	std::map<UINT_PTR, CString> m_mapTimerToFile;

	// 临界区保护（如果需要多线程访问）
	CRITICAL_SECTION m_cs;
};

