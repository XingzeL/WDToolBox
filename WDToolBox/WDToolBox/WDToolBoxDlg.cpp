// WDToolBoxDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "WDToolBox.h"
#include "WDToolBoxDlg.h"
#include "afxdialogex.h"
#include <shlwapi.h>
#include <shellapi.h>
#include "WriteWorkLogDlg.h"
#include "WorkLogWriter.h"

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
    , m_toolManager(&m_toolConfigReader, nullptr)  // 注入 ConfigReader，Executor 使用默认值
    , m_workLogger(&m_logConfigReader, nullptr)    // 注入 ConfigReader，Executor 使用默认值
    , m_nToolConfigTimerId(0)
    , m_nLogConfigTimerId(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_nCategoryListWidth = 200;  // 默认左侧列表宽度
}

CWDToolBoxDlg::~CWDToolBoxDlg()
{
    // 停止配置文件监控
    if (m_nToolConfigTimerId != 0)
    {
        m_configWatcher.StopWatch(m_hWnd, m_nToolConfigTimerId);
    }
    if (m_nLogConfigTimerId != 0)
    {
        m_configWatcher.StopWatch(m_hWnd, m_nLogConfigTimerId);
    }
}

void CWDToolBoxDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    // 注意：控件是通过代码动态创建的，不需要使用 DDX_Control
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
    ON_WM_CAPTURECHANGED()
    ON_WM_SETCURSOR()
    ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_LIST, &CWDToolBoxDlg::OnLvnItemchangedCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_TOOL_LIST, &CWDToolBoxDlg::OnNMDblclkToolList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOG_CATEGORY_LIST, &CWDToolBoxDlg::OnLvnItemchangedLogCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_LOG_LIBRARY_LIST, &CWDToolBoxDlg::OnNMDblclkLogLibraryList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CWDToolBoxDlg::OnTcnSelchangeTabMain)
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

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    // 执行此操作
    SetIcon(m_hIcon, TRUE);   // 设置大图标
    SetIcon(m_hIcon, FALSE);  // 设置小图标

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

    // 获取分页区域并创建分页控件
    CRect rectPage;
    GetClientRect(&rectPage);
    m_tabCtrl.GetClientRect(&rectPage);
    m_tabCtrl.AdjustRect(FALSE, &rectPage);
    rectPage.OffsetRect(10, 35);
    rectPage.right -= 10;
    rectPage.bottom -= 10;

    // 创建分页控件
    m_toolManagerPage.CreateControls(this, rectPage);
    m_workLogPage.CreateControls(this, rectPage);

    // 设置初始宽度
    m_toolManagerPage.SetLeftWidth(m_nCategoryListWidth);
    m_workLogPage.SetLeftWidth(m_nCategoryListWidth);

    // 设置 Manager 指针（用于观察者模式）
    m_toolManagerPage.SetToolManager(&m_toolManager);
    m_workLogPage.SetWorkLogManager(&m_workLogger);

    // 注册观察者
    m_toolManager.AddObserver(&m_toolManagerPage);
    m_workLogger.AddObserver(&m_workLogPage);

    // 加载工具分类（观察者会自动更新UI）
    LoadToolCategories();

    // 启动配置文件监控
    StartConfigFileWatcher();

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
// 来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
// 这将由框架自动完成。

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

    // 创建标签页控件
    CRect rectTab(10, 10, rect.Width() - 10, rect.Height() - 10);
    m_tabCtrl.Create(WS_CHILD | WS_VISIBLE | TCS_TABS | TCS_FOCUSNEVER,
        rectTab, this, IDC_TAB_MAIN);
    m_tabCtrl.InsertItem(0, _T("工具管理器"));
    m_tabCtrl.InsertItem(1, _T("工作日志"));
}


// 加载工具分类
void CWDToolBoxDlg::LoadToolCategories()
{
    // 从配置文件加载工具，如果失败则使用默认配置
    if (!m_toolManager.LoadFromConfig(_T("")))
    {
        // 配置文件不存在或加载失败，将使用默认配置
        MessageBox(_T("配置文件不存在或加载失败，将使用默认配置。"), _T("提示"), MB_OK | MB_ICONINFORMATION);
        m_toolManager.LoadDefaultTools();
    }

    // 加载图标（这会创建图像列表）
    m_toolManager.LoadToolIcons(m_imageList);

    // 将图像列表设置到工具列表控件
    CListCtrl* pToolList = m_toolManagerPage.GetToolList();
    if (pToolList)
    {
        pToolList->SetImageList(&m_imageList, LVSIL_NORMAL);
    }

    // 注意：分类列表和工具列表的填充现在由观察者模式自动处理
    // 但首次加载时，我们需要手动触发一次刷新（因为观察者可能还未注册）
    // 实际上，LoadFromConfig 会触发 ConfigLoaded 事件，观察者会自动更新
    // 这里保留手动更新作为后备
    m_toolManagerPage.RefreshCategoryList();
}

// 更新工具列表
void CWDToolBoxDlg::UpdateToolList(const CString& strCategory)
{
    CListCtrl* pToolList = m_toolManagerPage.GetToolList();
    if (!pToolList)
        return;

    pToolList->DeleteAllItems();

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

        int nIndex = pToolList->InsertItem(&lvItem);
        pToolList->SetItemData(nIndex, (DWORD_PTR)&tools[i]);
    }
}

// 分类列表选择改变事件
void CWDToolBoxDlg::OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (pNMLV->uChanged & LVIF_STATE && pNMLV->uNewState & LVIS_SELECTED)
    {
        CListCtrl* pCategoryList = m_toolManagerPage.GetCategoryList();
        if (pCategoryList)
        {
            CString strCategory = pCategoryList->GetItemText(pNMLV->iItem, 0);
            UpdateToolList(strCategory);
        }
    }

    *pResult = 0;
}

// 工具列表双击事件
void CWDToolBoxDlg::OnNMDblclkToolList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    if (pNMItemActivate->iItem >= 0)
    {
        CListCtrl* pToolList = m_toolManagerPage.GetToolList();
        if (pToolList)
        {
            ToolInfo* pTool = (ToolInfo*)pToolList->GetItemData(pNMItemActivate->iItem);
            if (pTool != NULL)
            {
                m_toolManager.ExecuteTool(*pTool); // 委托给 Manager 执行工具
            }
        }
    }

    *pResult = 0;
}

// 窗口大小改变事件
void CWDToolBoxDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if (nType == SIZE_MINIMIZED)
        return;

    if (m_tabCtrl.GetSafeHwnd())
    {
        // 更新标签页控件大小
        CRect rectTab(10, 10, cx - 10, cy - 10);
        m_tabCtrl.MoveWindow(&rectTab);

        // 获取分页内容区域
        CRect rectPage;
        m_tabCtrl.GetClientRect(&rectPage);
        m_tabCtrl.AdjustRect(FALSE, &rectPage);
        rectPage.OffsetRect(10, 35);  // 相对于标签页的偏移
        rectPage.right = cx - 20;
        rectPage.bottom = cy - 20;

        // 计算最大宽度限制
        int maxWidth = min(rectPage.Width() / 2, rectPage.Width() - 200);
        if (maxWidth < 150)
            maxWidth = 150;

        // 确保当前宽度在合理范围内
        if (m_nCategoryListWidth < 150)
            m_nCategoryListWidth = 150;
        if (m_nCategoryListWidth > maxWidth)
            m_nCategoryListWidth = maxWidth;

        // 更新所有分页的布局
        m_toolManagerPage.UpdateLayout(rectPage, m_nCategoryListWidth);
        m_workLogPage.UpdateLayout(rectPage, m_nCategoryListWidth);
    }
}

// 鼠标左键按下事件（用于拖动分割条）
void CWDToolBoxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // 委托给当前分页处理
    int nCurSel = m_tabCtrl.GetCurSel();
    ITabPage* pCurrentPage = nullptr;
    if (nCurSel == 0)
        pCurrentPage = &m_toolManagerPage;
    else if (nCurSel == 1)
        pCurrentPage = &m_workLogPage;

    if (pCurrentPage && pCurrentPage->OnLButtonDown(point, m_nCategoryListWidth))
    {
        // 需要更新布局
        CRect rect;
        GetClientRect(&rect);
        OnSize(0, rect.Width(), rect.Height());
        return;
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}

// 鼠标移动事件（拖动分割条时调整宽度）
void CWDToolBoxDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect clientRect;
    GetClientRect(&clientRect);

    // 委托给当前分页处理
    int nCurSel = m_tabCtrl.GetCurSel();
    ITabPage* pCurrentPage = nullptr;
    if (nCurSel == 0)
        pCurrentPage = &m_toolManagerPage;
    else if (nCurSel == 1)
        pCurrentPage = &m_workLogPage;

    if (pCurrentPage && pCurrentPage->OnMouseMove(point, m_nCategoryListWidth, clientRect))
    {
        // 需要更新布局
        OnSize(0, clientRect.Width(), clientRect.Height());
        return;
    }

    CDialogEx::OnMouseMove(nFlags, point);
}

// 鼠标左键释放事件（结束拖动）
void CWDToolBoxDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // 委托给当前分页处理
    int nCurSel = m_tabCtrl.GetCurSel();
    if (nCurSel == 0)
        m_toolManagerPage.OnLButtonUp();
    else if (nCurSel == 1)
        m_workLogPage.OnLButtonUp();

    CDialogEx::OnLButtonUp(nFlags, point);
}

// 鼠标捕获改变事件（处理窗口大小调整时的鼠标捕获丢失）
void CWDToolBoxDlg::OnCaptureChanged(CWnd* pWnd)
{
    // 委托给当前分页处理
    int nCurSel = m_tabCtrl.GetCurSel();
    if (nCurSel == 0)
        m_toolManagerPage.OnCaptureChanged();
    else if (nCurSel == 1)
        m_workLogPage.OnCaptureChanged();

    CDialogEx::OnCaptureChanged(pWnd);
}

// 设置光标（在分割条上显示调整光标）
BOOL CWDToolBoxDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // 获取鼠标位置
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);

    // 委托给当前分页处理
    int nCurSel = m_tabCtrl.GetCurSel();
    if (nCurSel == 0)
    {
        if (m_toolManagerPage.OnSetCursor(point))
            return TRUE;
    }
    else if (nCurSel == 1)
    {
        if (m_workLogPage.OnSetCursor(point))
            return TRUE;
    }

    return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

// 分页切换事件处理
void CWDToolBoxDlg::OnTcnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult)
{
    int nCurSel = m_tabCtrl.GetCurSel();
    ShowTabPage(nCurSel);
    *pResult = 0;
}

// 显示指定分页
void CWDToolBoxDlg::ShowTabPage(int nPage)
{
    // 隐藏所有分页
    m_toolManagerPage.Hide();
    m_workLogPage.Hide();

    switch (nPage)
    {
    case 0: // 工具管理器分页
        m_toolManagerPage.Show();
        break;
    case 1: // 工作日志分页
        m_workLogPage.Show();
        LoadLogCategories();
        break;
    }
}

// 加载日志分类
void CWDToolBoxDlg::LoadLogCategories()
{
    // 从配置文件加载日志库，如果失败则使用默认配置
    if (!m_workLogger.LoadFromConfig(_T("")))
    {
        // 配置文件不存在或加载失败，将使用默认配置
        MessageBox(_T("日志配置文件不存在或加载失败，将使用默认配置。"), _T("提示"), MB_OK | MB_ICONINFORMATION);
        m_workLogger.LoadDefaultLibraries();
    }

    // 注意：分类列表和库列表的填充现在由观察者模式自动处理
    // LoadFromConfig 会触发 ConfigLoaded 事件，观察者会自动更新
    // 这里保留手动更新作为后备
    m_workLogPage.RefreshCategoryList();
}

// 更新日志库列表
void CWDToolBoxDlg::UpdateLogLibraryList(const CString& strCategory)
{
    CListCtrl* pLibraryList = m_workLogPage.GetLibraryList();
    if (!pLibraryList)
        return;

    pLibraryList->DeleteAllItems();

    std::vector<LogLibraryInfo>& libraries = m_workLogger.GetLibrariesByCategory(strCategory);
    for (size_t i = 0; i < libraries.size(); i++)
    {
        pLibraryList->InsertItem((int)i, libraries[i].strName);
    }
}

// 日志分类列表选择改变事件
void CWDToolBoxDlg::OnLvnItemchangedLogCategoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (pNMLV->uChanged & LVIF_STATE && pNMLV->uNewState & LVIS_SELECTED)
    {
        CListCtrl* pCategoryList = m_workLogPage.GetCategoryList();
        if (pCategoryList)
        {
            CString strCategory = pCategoryList->GetItemText(pNMLV->iItem, 0);
            UpdateLogLibraryList(strCategory);
        }
    }

    *pResult = 0;
}

// 日志库列表双击事件
void CWDToolBoxDlg::OnNMDblclkLogLibraryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    if (pNMItemActivate->iItem >= 0)
    {
        CListCtrl* pCategoryList = m_workLogPage.GetCategoryList();
        CListCtrl* pLibraryList = m_workLogPage.GetLibraryList();

        if (pCategoryList && pLibraryList)
        {
            CString strLibraryInfo = pLibraryList->GetItemText(pNMItemActivate->iItem, 0);
            int nSel = pCategoryList->GetNextItem(-1, LVNI_SELECTED);
            CString strProjectInfo = (nSel >= 0) ? pCategoryList->GetItemText(nSel, 0) : _T("");
            CWriteWorkLogDlg dlg(this);
            dlg.SetProjectInfo(strProjectInfo);
            dlg.SetLibraryInfo(strLibraryInfo);
            if (dlg.DoModal() == IDOK)
            {
                CString strLogContent = dlg.GetLogContent();
                CString strLogContenth = dlg.GetLogContenth();

                // 写入结构化数据版本到 logs.txt
                if (!strLogContent.IsEmpty())
                {
                    m_workLogger.WriteLog(strLogContent); // 委托给 Manager 写入日志
                }

                // 写入人类可读版本到 logsh.txt
                if (!strLogContenth.IsEmpty())
                {
                    // 直接使用 WorkLogWriter 的 ExecuteH 方法写入人类可读版本
                    CWorkLogWriter writer;
                    writer.ExecuteH(strLogContenth);
                }
            }
        }
    }

    *pResult = 0;
}

void CWDToolBoxDlg::StartConfigFileWatcher()
{
    // 获取工具配置文件路径
    CString strToolConfigPath = m_toolManager.GetDefaultConfigPath();
    if (!strToolConfigPath.IsEmpty() && PathFileExists(strToolConfigPath))
    {
        // 启动工具配置文件监控
        m_nToolConfigTimerId = m_configWatcher.StartWatch(m_hWnd, strToolConfigPath,
            [this](const CString& strFilePath) {
                // 文件变化时重新加载配置
                m_toolManager.LoadFromConfig(strFilePath);
                // 观察者会自动更新UI
            });
    }

    // 获取日志配置文件路径
    CString strLogConfigPath = m_workLogger.GetDefaultConfigPath();
    if (!strLogConfigPath.IsEmpty() && PathFileExists(strLogConfigPath))
    {
        // 启动日志配置文件监控
        m_nLogConfigTimerId = m_configWatcher.StartWatch(m_hWnd, strLogConfigPath,
            [this](const CString& strFilePath) {
                // 文件变化时重新加载配置
                m_workLogger.LoadFromConfig(strFilePath);
                // 观察者会自动更新UI
            });
    }
}

void CWDToolBoxDlg::OnTimer(UINT_PTR nIDEvent)
{
    // 定时器事件由 ConfigFileWatcher 的 TimerProc 处理（使用系统定时器）
    // 这里不需要额外处理
    CDialogEx::OnTimer(nIDEvent);
}
