
// WDToolBoxDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "WDToolBox.h"
#include "WDToolBoxDlg.h"
#include "afxdialogex.h"
#include <shlwapi.h>
#include <shellapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWDToolBoxDlg 对话框



CWDToolBoxDlg::CWDToolBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WDTOOLBOX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCategoryListWidth = 200;  // 默认左侧列表宽度
	m_bDragging = FALSE;
	m_nDragStartX = 0;
	m_nDragStartWidth = 200;
}

void CWDToolBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// 注意：控件是通过代码动态创建的，不需要使用DDX_Control
	// DDX_Control(pDX, IDC_CATEGORY_LIST, m_listCategory);
	// DDX_Control(pDX, IDC_TOOL_LIST, m_listTool);
}

BEGIN_MESSAGE_MAP(CWDToolBoxDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_LIST, &CWDToolBoxDlg::OnLvnItemchangedCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_TOOL_LIST, &CWDToolBoxDlg::OnNMDblclkToolList)
END_MESSAGE_MAP()


// CWDToolBoxDlg 消息处理程序

BOOL CWDToolBoxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 使对话框可以调整大小，并添加最小化和最大化按钮
	ModifyStyle(0, WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

	// 隐藏不需要的确定和取消按钮
	CWnd* pOkBtn = GetDlgItem(IDOK);
	if (pOkBtn != nullptr)
	{
		pOkBtn->ShowWindow(SW_HIDE);
	}
	CWnd* pCancelBtn = GetDlgItem(IDCANCEL);
	if (pCancelBtn != nullptr)
	{
		pCancelBtn->ShowWindow(SW_HIDE);
	}

	// 初始化控件
	InitializeControls();

	// 加载工具分类
	LoadToolCategories();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWDToolBoxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWDToolBoxDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWDToolBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 初始化控件
void CWDToolBoxDlg::InitializeControls()
{
	CRect rect;
	GetClientRect(&rect);

	// 创建左侧分类列表控件
	CRect rectCategory(10, 10, 10 + m_nCategoryListWidth, rect.Height() - 10);
	m_listCategory.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
		rectCategory, this, IDC_CATEGORY_LIST);
	m_listCategory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCategory.InsertColumn(0, _T("工具分类"), LVCFMT_LEFT, m_nCategoryListWidth - 20);

	// 创建分割条（用于调整左右控件宽度）
	CRect rectSplitter(10 + m_nCategoryListWidth, 10, 10 + m_nCategoryListWidth + 4, rect.Height() - 10);
	m_splitter.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT, rectSplitter, this, IDC_SPLITTER);
	m_splitter.SetWindowPos(&m_listCategory, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// 创建右侧工具图标列表控件
	CRect rectTool(10 + m_nCategoryListWidth + 4 + 6, 10, rect.Width() - 10, rect.Height() - 10);
	m_listTool.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ICON | LVS_AUTOARRANGE,
		rectTool, this, IDC_TOOL_LIST);
	m_listTool.SetExtendedStyle(LVS_EX_DOUBLEBUFFER);

	// 注意：图像列表将在LoadToolCategories中创建和设置
}

// 从配置文件加载工具
void CWDToolBoxDlg::LoadToolsFromConfig(const CString& strConfigPath)
{
	// 获取配置文件路径（如果未指定，使用程序目录下的 tools.ini）
	CString strIniPath = strConfigPath;
	if (strIniPath.IsEmpty())
	{
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
	}

	// 检查配置文件是否存在
	if (!PathFileExists(strIniPath))
	{
		// 如果配置文件不存在，使用默认配置
		MessageBox(_T("配置文件不存在，将使用默认配置。\n配置文件路径：") + strIniPath, 
			_T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	// 调试：显示配置文件路径（可以注释掉）
	// MessageBox(_T("正在读取配置文件：\n") + strIniPath, _T("调试"), MB_OK);

	// 读取UTF-8编码的文件内容（支持Unicode和多字节字符集）
	HANDLE hFile = CreateFile(strIniPath, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(_T("无法打开配置文件。"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	// 获取文件大小
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0 || dwFileSize > 1024 * 1024) // 限制最大1MB
	{
		CloseHandle(hFile);
		MessageBox(_T("配置文件大小无效。"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	// 读取文件内容
	char* pBuffer = new char[dwFileSize + 1];
	DWORD dwBytesRead = 0;
	if (!ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, NULL))
	{
		delete[] pBuffer;
		CloseHandle(hFile);
		MessageBox(_T("读取配置文件失败。"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}
	pBuffer[dwBytesRead] = 0;
	CloseHandle(hFile);

	// 检测并跳过BOM（UTF-8 BOM: EF BB BF）
	int nStartPos = 0;
	if (dwBytesRead >= 3 && (unsigned char)pBuffer[0] == 0xEF && 
		(unsigned char)pBuffer[1] == 0xBB && (unsigned char)pBuffer[2] == 0xBF)
	{
		nStartPos = 3;
	}

	// 将UTF-8转换为宽字符（Unicode）或多字节字符
#ifdef _UNICODE
	// Unicode模式：UTF-8 -> wchar_t
	int nWideLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, 
		dwBytesRead - nStartPos, NULL, 0);
	if (nWideLen <= 0)
	{
		delete[] pBuffer;
		MessageBox(_T("配置文件编码错误（需要UTF-8编码）。"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	wchar_t* pWideBuffer = new wchar_t[nWideLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, dwBytesRead - nStartPos, 
		pWideBuffer, nWideLen);
	pWideBuffer[nWideLen] = 0;
	delete[] pBuffer;

	// 解析INI文件内容
	CString strContent = pWideBuffer;
	delete[] pWideBuffer;
#else
	// 多字节模式：UTF-8 -> 多字节字符（GBK）
	// 先转换为Unicode，再转换为多字节
	int nWideLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, 
		dwBytesRead - nStartPos, NULL, 0);
	if (nWideLen <= 0)
	{
		delete[] pBuffer;
		MessageBox(_T("配置文件编码错误（需要UTF-8编码）。"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	wchar_t* pWideBuffer = new wchar_t[nWideLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, pBuffer + nStartPos, dwBytesRead - nStartPos, 
		pWideBuffer, nWideLen);
	pWideBuffer[nWideLen] = 0;
	delete[] pBuffer;

	// 将Unicode转换为多字节字符
	int nMultiByteLen = WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nWideLen, NULL, 0, NULL, NULL);
	if (nMultiByteLen <= 0)
	{
		delete[] pWideBuffer;
		MessageBox(_T("字符转换失败。"), _T("错误"), MB_OK | MB_ICONERROR);
		return;
	}

	char* pMultiByteBuffer = new char[nMultiByteLen + 1];
	WideCharToMultiByte(CP_ACP, 0, pWideBuffer, nWideLen, pMultiByteBuffer, nMultiByteLen, NULL, NULL);
	pMultiByteBuffer[nMultiByteLen] = 0;
	delete[] pWideBuffer;

	// 解析INI文件内容
	CString strContent = pMultiByteBuffer;
	delete[] pMultiByteBuffer;
#endif

	CString strCurrentCategory;
	int nPos = 0;
	CString strLine;

	// 逐行解析
	while (nPos < strContent.GetLength())
	{
		int nLineEnd = strContent.Find(_T('\n'), nPos);
		if (nLineEnd == -1)
		{
			strLine = strContent.Mid(nPos);
			nPos = strContent.GetLength();
		}
		else
		{
			strLine = strContent.Mid(nPos, nLineEnd - nPos);
			nPos = nLineEnd + 1;
		}

		// 去除回车符和前后空格
		strLine.Trim();
		if (strLine.IsEmpty())
			continue;

		// 检查是否是分类行 [分类名]
		if (strLine.GetLength() > 2 && strLine[0] == _T('[') && 
			strLine[strLine.GetLength() - 1] == _T(']'))
		{
			strCurrentCategory = strLine.Mid(1, strLine.GetLength() - 2);
			strCurrentCategory.Trim();
		}
		// 检查是否是键值对 工具名称=工具路径
		else if (!strCurrentCategory.IsEmpty())
		{
			int nEqualPos = strLine.Find(_T('='));
			if (nEqualPos > 0)
			{
				CString strToolName = strLine.Left(nEqualPos);
				CString strToolPath = strLine.Mid(nEqualPos + 1);

				// 去除空格和引号
				strToolName.Trim();
				strToolPath.Trim();
				if (strToolPath.GetLength() > 0 && strToolPath[0] == _T('"'))
				{
					strToolPath = strToolPath.Mid(1);
				}
				if (strToolPath.GetLength() > 0 && strToolPath[strToolPath.GetLength() - 1] == _T('"'))
				{
					strToolPath = strToolPath.Left(strToolPath.GetLength() - 1);
				}

				// 添加工具
				if (!strToolName.IsEmpty() && !strToolPath.IsEmpty())
				{
					m_toolManager.AddTool(strCurrentCategory, strToolName, strToolPath);
				}
			}
		}
	}
}

// 加载工具分类
void CWDToolBoxDlg::LoadToolCategories()
{
	// 从配置文件加载工具
	LoadToolsFromConfig(_T(""));

	// 如果配置文件为空或不存在，使用默认配置
	std::vector<CString> tempCategories;
	m_toolManager.GetAllCategories(tempCategories);
	if (tempCategories.empty())
	{
		// 默认配置（作为后备）
		m_toolManager.AddTool(_T("系统工具"), _T("任务管理器"), _T("C:\\Windows\\System32\\taskmgr.exe"));
		m_toolManager.AddTool(_T("系统工具"), _T("注册表编辑器"), _T("C:\\Windows\\regedit.exe"));
		m_toolManager.AddTool(_T("系统工具"), _T("系统配置"), _T("C:\\Windows\\System32\\msconfig.exe"));
		m_toolManager.AddTool(_T("系统工具"), _T("设备管理器"), _T("C:\\Windows\\System32\\devmgmt.msc"));
		m_toolManager.AddTool(_T("系统工具"), _T("服务"), _T("C:\\Windows\\System32\\services.msc"));
		m_toolManager.AddTool(_T("系统工具"), _T("证书管理"), _T("C:\\Windows\\System32\\certmgr.msc"));

		m_toolManager.AddTool(_T("磁盘工具"), _T("磁盘管理"), _T("C:\\Windows\\System32\\diskmgmt.msc"));
		m_toolManager.AddTool(_T("磁盘工具"), _T("磁盘清理"), _T("C:\\Windows\\System32\\cleanmgr.exe"));

		m_toolManager.AddTool(_T("网络工具"), _T("网络连接"), _T("C:\\Windows\\System32\\ncpa.cpl"));
		m_toolManager.AddTool(_T("网络工具"), _T("命令提示符"), _T("C:\\Windows\\System32\\cmd.exe"));
	}

	// 加载图标（这会创建图像列表）
	m_toolManager.LoadToolIcons(m_imageList);
	
	// 将图像列表设置到工具列表控件
	m_listTool.SetImageList(&m_imageList, LVSIL_NORMAL);

	// 填充分类列表
	std::vector<CString> categories;
	m_toolManager.GetAllCategories(categories);

	m_listCategory.DeleteAllItems();
	for (size_t i = 0; i < categories.size(); i++)
	{
		int nIndex = m_listCategory.InsertItem((int)i, categories[i]);
		m_listCategory.SetItemData(nIndex, i);
	}

	// 默认选择第一项
	if (m_listCategory.GetItemCount() > 0)
	{
		m_listCategory.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		UpdateToolList(categories[0]);
	}
}

// 更新工具列表
void CWDToolBoxDlg::UpdateToolList(const CString& strCategory)
{
	m_listTool.DeleteAllItems();

	std::vector<ToolInfo>& tools = m_toolManager.GetToolsByCategory(strCategory);
	for (size_t i = 0; i < tools.size(); i++)
	{
		LVITEM lvItem = { 0 };
		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem = (int)i;
		lvItem.iSubItem = 0;
		lvItem.pszText = (LPTSTR)(LPCTSTR)tools[i].strName;
		lvItem.iImage = tools[i].nIconIndex;
		lvItem.lParam = (LPARAM)&tools[i];

		int nIndex = m_listTool.InsertItem(&lvItem);
		m_listTool.SetItemData(nIndex, (DWORD_PTR)&tools[i]);
	}
}

// 分类列表选择改变事件
void CWDToolBoxDlg::OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	if (pNMLV->uChanged & LVIF_STATE && pNMLV->uNewState & LVIS_SELECTED)
	{
		CString strCategory = m_listCategory.GetItemText(pNMLV->iItem, 0);
		UpdateToolList(strCategory);
	}

	*pResult = 0;
}

// 工具列表双击事件
void CWDToolBoxDlg::OnNMDblclkToolList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iItem >= 0)
	{
		ToolInfo* pTool = (ToolInfo*)m_listTool.GetItemData(pNMItemActivate->iItem);
		if (pTool != NULL)
		{
			m_launcher.Execute(pTool->strPath); // 进程启动器启动工具
		}
	}

	*pResult = 0;
}

// 窗口大小改变事件
void CWDToolBoxDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_listCategory.GetSafeHwnd() && m_listTool.GetSafeHwnd() && m_splitter.GetSafeHwnd())
	{
		// 确保左侧列表宽度在合理范围内（最小150，最大不超过窗口宽度的50%）
		if (m_nCategoryListWidth < 150)
			m_nCategoryListWidth = 150;
		if (m_nCategoryListWidth > cx / 2)
			m_nCategoryListWidth = cx / 2;

		// 调整左侧分类列表大小
		m_listCategory.SetWindowPos(NULL, 10, 10, m_nCategoryListWidth, cy - 20, SWP_NOZORDER);
		// 更新列宽以适应新的控件宽度
		m_listCategory.SetColumnWidth(0, m_nCategoryListWidth - 20);

		// 调整分割条位置
		m_splitter.SetWindowPos(NULL, 10 + m_nCategoryListWidth, 10, 4, cy - 20, SWP_NOZORDER);

		// 调整右侧工具列表大小
		int nRightListLeft = 10 + m_nCategoryListWidth + 4 + 6;
		int nRightListWidth = cx - nRightListLeft - 10;
		if (nRightListWidth > 0)
		{
			m_listTool.SetWindowPos(NULL, nRightListLeft, 10, nRightListWidth, cy - 20, SWP_NOZORDER);
		}
	}
}

// 鼠标左键按下事件（用于拖动分割条）
void CWDToolBoxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rectSplitter;
	if (m_splitter.GetSafeHwnd())
	{
		m_splitter.GetWindowRect(&rectSplitter);
		ScreenToClient(&rectSplitter);
		
		// 扩大可拖动区域（左右各扩展5像素）
		rectSplitter.InflateRect(5, 0);
		
		if (rectSplitter.PtInRect(point))
		{
			m_bDragging = TRUE;
			m_nDragStartX = point.x;
			m_nDragStartWidth = m_nCategoryListWidth;
			SetCapture();  // 捕获鼠标消息
			return;
		}
	}
	
	CDialogEx::OnLButtonDown(nFlags, point);
}

// 鼠标移动事件（拖动分割条时调整宽度）
void CWDToolBoxDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		// 计算新的宽度
		int nDeltaX = point.x - m_nDragStartX;
		int nNewWidth = m_nDragStartWidth + nDeltaX;
		
		// 限制宽度范围（最小150，最大不超过窗口宽度的50%）
		CRect rect;
		GetClientRect(&rect);
		if (nNewWidth < 150)
			nNewWidth = 150;
		if (nNewWidth > rect.Width() / 2)
			nNewWidth = rect.Width() / 2;
		
		// 更新宽度并重新布局
		if (nNewWidth != m_nCategoryListWidth)
		{
			m_nCategoryListWidth = nNewWidth;
			OnSize(0, rect.Width(), rect.Height());
		}
	}
	else
	{
		// 检查鼠标是否在分割条上，改变光标
		CRect rectSplitter;
		if (m_splitter.GetSafeHwnd())
		{
			m_splitter.GetWindowRect(&rectSplitter);
			ScreenToClient(&rectSplitter);
			rectSplitter.InflateRect(5, 0);
			
			if (rectSplitter.PtInRect(point))
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));  // 左右调整光标
			}
		}
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}

// 鼠标左键释放事件（结束拖动）
void CWDToolBoxDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		ReleaseCapture();  // 释放鼠标捕获
	}
	
	CDialogEx::OnLButtonUp(nFlags, point);
}

// 设置光标（在分割条上显示调整光标）
BOOL CWDToolBoxDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (pWnd == &m_splitter || pWnd->GetDlgCtrlID() == IDC_SPLITTER)
	{
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));  // 左右调整光标
		return TRUE;
	}
	
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


