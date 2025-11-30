
// WDToolBox.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "WDToolBox.h"
#include "WDToolBoxDlg.h"
#include <windows.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWDToolBoxApp

BEGIN_MESSAGE_MAP(CWDToolBoxApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWDToolBoxApp 构造

CWDToolBoxApp::CWDToolBoxApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CWDToolBoxApp 对象

CWDToolBoxApp theApp;


// CWDToolBoxApp 初始化

BOOL CWDToolBoxApp::InitInstance()
{
	// 检查是否已有实例在运行（全局单实例）
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("WDToolBox_SingleInstance_Mutex"));
	DWORD dwLastError = GetLastError();

	if (hMutex != NULL && dwLastError == ERROR_ALREADY_EXISTS)
	{
		// 已有实例在运行，释放当前互斥体句柄
		CloseHandle(hMutex);

		// 尝试激活已存在的窗口
		// 遍历所有窗口，查找属于其他 WDToolBox 进程的主窗口
		struct FindWindowData
		{
			DWORD dwCurrentProcessId;
			HWND hFoundWnd;
		};

		FindWindowData data;
		data.dwCurrentProcessId = GetCurrentProcessId();
		data.hFoundWnd = NULL;

		EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
			FindWindowData* pData = (FindWindowData*)lParam;
			DWORD dwProcessId = 0;
			GetWindowThreadProcessId(hWnd, &dwProcessId);

			// 只查找属于其他进程的可见窗口
			if (dwProcessId != pData->dwCurrentProcessId && IsWindowVisible(hWnd))
			{
				// 检查进程名是否为 WDToolBox.exe
				TCHAR szProcessName[MAX_PATH] = { 0 };
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
				if (hProcess != NULL)
				{
					HMODULE hMod;
					DWORD cbNeeded;
					if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
					{
						if (GetModuleBaseName(hProcess, hMod, szProcessName, MAX_PATH) > 0)
						{
							if (_tcsicmp(szProcessName, _T("WDToolBox.exe")) == 0)
							{
								// 找到 WDToolBox 窗口
								pData->hFoundWnd = hWnd;
								CloseHandle(hProcess);
								return FALSE;  // 停止枚举
							}
						}
					}
					CloseHandle(hProcess);
				}
			}
			return TRUE;  // 继续枚举
		}, (LPARAM)&data);

		if (data.hFoundWnd != NULL && IsWindow(data.hFoundWnd))
		{
			// 如果窗口最小化，恢复它
			if (IsIconic(data.hFoundWnd))
			{
				ShowWindow(data.hFoundWnd, SW_RESTORE);
			}
			// 将窗口置于前台
			SetForegroundWindow(data.hFoundWnd);
			BringWindowToTop(data.hFoundWnd);
		}

		// 退出当前实例
		return FALSE;
	}

	// 如果应用程序存在以下情况，Windows XP 上需要 InitCommonControlsEx()
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CWDToolBoxDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

