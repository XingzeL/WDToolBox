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
    ON_WM_SETCURSOR()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_LIST, &CWDToolBoxDlg::OnLvnItemchangedCategoryList)
    ON_NOTIFY(NM_DBLCLK, IDC_TOOL_LIST, &CWDToolBoxDlg::OnNMDblclkToolList)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CWDToolBoxDlg::OnTcnSelchangeTabMain)
    ON_BN_CLICKED(IDC_BTN_ADD_LOG, &CWDToolBoxDlg::OnBnClickedBtnAddLog)
    ON_BN_CLICKED(IDC_BTN_SAVE_LOG, &CWDToolBoxDlg::OnBnClickedBtnSaveLog)
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

    // 创建工具管理器分页控件
    CRect rectPage;
    m_tabCtrl.GetClientRect(&rectPage);
    m_tabCtrl.AdjustRect(FALSE, &rectPage);
    rectPage.OffsetRect(10, 35); // 相对于标签页的偏移
    rectPage.right -= 10;
    rectPage.bottom -= 10;

    // 左侧分类列表
    CRect rectCategory(rectPage.left, rectPage.top,
        rectPage.left + m_nCategoryListWidth, rectPage.bottom);
    m_listCategory.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        rectCategory, this, IDC_CATEGORY_LIST);
    m_listCategory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listCategory.InsertColumn(0, _T("工具分类"), LVCFMT_LEFT, m_nCategoryListWidth - 20);

    // 分割条
    CRect rectSplitter(rectCategory.right, rectPage.top, rectCategory.right + 4, rectPage.bottom);
    m_splitter.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT,
        rectSplitter, this, IDC_SPLITTER);

    // 右侧工具列表
    CRect rectTool(rectSplitter.right + 6, rectPage.top, rectPage.right, rectPage.bottom);
    m_listTool.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ICON | LVS_AUTOARRANGE,
        rectTool, this, IDC_TOOL_LIST);
    m_listTool.SetExtendedStyle(LVS_EX_DOUBLEBUFFER);

    // 创建工作日志分页控件（初始隐藏）
    CRect rectWorkLog(rectPage);
    m_listWorkLog.Create(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        CRect(rectWorkLog.left, rectWorkLog.top, rectWorkLog.right, rectWorkLog.bottom - 100),
        this, IDC_WORKLOG_LIST);
    m_listWorkLog.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listWorkLog.InsertColumn(0, _T("时间"), LVCFMT_LEFT, 150);
    m_listWorkLog.InsertColumn(1, _T("内容"), LVCFMT_LEFT, rectWorkLog.Width() - 170);

    m_editWorkLog.Create(WS_CHILD | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
        CRect(rectWorkLog.left, rectWorkLog.bottom - 95, rectWorkLog.right - 120, rectWorkLog.bottom - 40),
        this, IDC_WORKLOG_EDIT);

    m_btnAddLog.Create(_T("添加日志"), WS_CHILD | BS_PUSHBUTTON,
        CRect(rectWorkLog.right - 110, rectWorkLog.bottom - 95, rectWorkLog.right - 10, rectWorkLog.bottom - 70),
        this, IDC_BTN_ADD_LOG);

    m_btnSaveLog.Create(_T("保存日志"), WS_CHILD | BS_PUSHBUTTON,
        CRect(rectWorkLog.right - 110, rectWorkLog.bottom - 65, rectWorkLog.right - 10, rectWorkLog.bottom - 40),
        this, IDC_BTN_SAVE_LOG);

    // 初始显示第一个分页
    ShowTabPage(0);
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

    if (nType == SIZE_MINIMIZED)
        return;

    if (m_tabCtrl.GetSafeHwnd())
    {
        // 调整标签页控件大小
        CRect rectTab(10, 10, cx - 10, cy - 10);
        m_tabCtrl.MoveWindow(&rectTab);

        // 获取分页内容区域
        CRect rectPage;
        m_tabCtrl.GetClientRect(&rectPage);
        m_tabCtrl.AdjustRect(FALSE, &rectPage);
        rectPage.OffsetRect(10, 35);
        rectPage.right = cx - 20;
        rectPage.bottom = cy - 20;

        // 确保左侧列表宽度在合理范围内（最小150，最大不超过窗口宽度的50%）
        if (m_nCategoryListWidth < 150)
            m_nCategoryListWidth = 150;
        if (m_nCategoryListWidth > rectPage.Width() / 2)
            m_nCategoryListWidth = rectPage.Width() / 2;

        // 调整工具管理器分页控件
        CRect rectCategory(rectPage.left, rectPage.top,
            rectPage.left + m_nCategoryListWidth, rectPage.bottom);
        m_listCategory.MoveWindow(&rectCategory);

        CRect rectSplitter(rectCategory.right, rectPage.top, rectCategory.right + 4, rectPage.bottom);
        m_splitter.MoveWindow(&rectSplitter);

        CRect rectTool(rectSplitter.right + 6, rectPage.top, rectPage.right, rectPage.bottom);
        m_listTool.MoveWindow(&rectTool);

        // 调整工作日志分页控件
        m_listWorkLog.MoveWindow(&CRect(rectPage.left, rectPage.top,
            rectPage.right, rectPage.bottom - 100));

        m_editWorkLog.MoveWindow(&CRect(rectPage.left, rectPage.bottom - 95,
            rectPage.right - 120, rectPage.bottom - 40));

        CRect rectBtn(rectPage.right - 110, rectPage.bottom - 95,
            rectPage.right - 10, rectPage.bottom - 70);
        m_btnAddLog.MoveWindow(&rectBtn);

        rectBtn.OffsetRect(0, 30);
        m_btnSaveLog.MoveWindow(&rectBtn);

        // 重新调整列宽
        if (m_listCategory.GetSafeHwnd())
        {
            m_listCategory.SetColumnWidth(0, m_nCategoryListWidth - 20);
        }
        if (m_listWorkLog.GetSafeHwnd())
        {
            m_listWorkLog.SetColumnWidth(0, 150);
            m_listWorkLog.SetColumnWidth(1, rectPage.Width() - 170);
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
    // 隐藏所有分页控件
    m_listCategory.ShowWindow(SW_HIDE);
    m_listTool.ShowWindow(SW_HIDE);
    m_splitter.ShowWindow(SW_HIDE);
    m_listWorkLog.ShowWindow(SW_HIDE);
    m_editWorkLog.ShowWindow(SW_HIDE);
    m_btnAddLog.ShowWindow(SW_HIDE);
    m_btnSaveLog.ShowWindow(SW_HIDE);

    switch (nPage)
    {
    case 0: // 工具管理器分页
        m_listCategory.ShowWindow(SW_SHOW);
        m_listTool.ShowWindow(SW_SHOW);
        m_splitter.ShowWindow(SW_SHOW);
        break;
    case 1: // 工作日志分页
        m_listWorkLog.ShowWindow(SW_SHOW);
        m_editWorkLog.ShowWindow(SW_SHOW);
        m_btnAddLog.ShowWindow(SW_SHOW);
        m_btnSaveLog.ShowWindow(SW_SHOW);
        InitializeWorkLogPage();
        break;
    }
}

// 初始化工作日志分页
void CWDToolBoxDlg::InitializeWorkLogPage()
{
    if (m_listWorkLog.GetItemCount() == 0)
    {
        LoadWorkLogData();
    }
}

// 加载工作日志数据
void CWDToolBoxDlg::LoadWorkLogData()
{
    m_listWorkLog.DeleteAllItems();
    std::vector<WorkLogEntry> logs;
    m_workLogger.GetAllLogs(logs);

    for (size_t i = 0; i < logs.size(); i++)
    {
        CString strTime = CTime(logs[i].time).Format(_T("%Y-%m-%d %H:%M:%S"));
        CString strContent = logs[i].content;

        m_listWorkLog.InsertItem((int)i, strTime);
        m_listWorkLog.SetItemText((int)i, 1, strContent);
    }
}

// 保存工作日志数据
void CWDToolBoxDlg::SaveWorkLogData()
{
    m_workLogger.SaveLogs();
}

// 添加日志按钮事件
void CWDToolBoxDlg::OnBnClickedBtnAddLog()
{
    CString strContent;
    m_editWorkLog.GetWindowText(strContent);
    if (!strContent.IsEmpty())
    {
        // 添加到列表
        int nItem = m_listWorkLog.GetItemCount();
        CTime time = CTime::GetCurrentTime();
        CString strTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));

        m_listWorkLog.InsertItem(nItem, strTime);
        m_listWorkLog.SetItemText(nItem, 1, strContent);

        // 清空编辑框
        m_editWorkLog.SetWindowText(_T(""));

        // 执行工作日志处理
        m_workLogger.Execute(strContent);
    }
}

// 保存日志按钮事件
void CWDToolBoxDlg::OnBnClickedBtnSaveLog()
{
    SaveWorkLogData();
    MessageBox(_T("工作日志已保存！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
}