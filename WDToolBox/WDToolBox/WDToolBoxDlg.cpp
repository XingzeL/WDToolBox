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
    ON_WM_CAPTURECHANGED()
    ON_WM_SETCURSOR()
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
    // 左侧日志分类列表
    CRect rectLogCategory(rectPage.left, rectPage.top,
        rectPage.left + m_nCategoryListWidth, rectPage.bottom);
    m_listLogCategory.Create(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        rectLogCategory, this, IDC_LOG_CATEGORY_LIST);
    m_listLogCategory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listLogCategory.InsertColumn(0, _T("日志分类"), LVCFMT_LEFT, m_nCategoryListWidth - 20);

    // 日志分页的分割条
    CRect rectLogSplitter(rectPage.left + m_nCategoryListWidth, rectPage.top,
        rectPage.left + m_nCategoryListWidth + 4, rectPage.bottom);
    m_logSplitter.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT,
        rectLogSplitter, this, IDC_LOG_SPLITTER);

    // 右侧日志库列表
    CRect rectLogLibrary(rectPage.left + m_nCategoryListWidth + 4 + 6, rectPage.top,
        rectPage.right, rectPage.bottom);
    m_listLogLibrary.Create(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        rectLogLibrary, this, IDC_LOG_LIBRARY_LIST);
    m_listLogLibrary.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_listLogLibrary.InsertColumn(0, _T("库名称"), LVCFMT_LEFT, rectPage.Width() - m_nCategoryListWidth - 30);

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

        // 确保左侧列表宽度在合理范围内（最小150，最大不超过窗口宽度的70%）
        if (m_nCategoryListWidth < 150)
            m_nCategoryListWidth = 150;
        // 为工作日志分页预留至少200像素宽度
        int maxWidth = min(rectPage.Width() / 2, rectPage.Width() - 200);
        if (m_nCategoryListWidth > maxWidth)
            m_nCategoryListWidth = maxWidth;

        // 调整工具管理器分页控件
        CRect rectCategory(rectPage.left, rectPage.top,
            rectPage.left + m_nCategoryListWidth, rectPage.bottom);
        m_listCategory.MoveWindow(&rectCategory);

        CRect rectSplitter(rectCategory.right, rectPage.top, rectCategory.right + 4, rectPage.bottom);
        m_splitter.MoveWindow(&rectSplitter);

        CRect rectTool(rectSplitter.right + 6, rectPage.top, rectPage.right, rectPage.bottom);
        m_listTool.MoveWindow(&rectTool);

        // 调整工作日志分页控件
        CRect rectLogCategory(rectPage.left, rectPage.top,
            rectPage.left + m_nCategoryListWidth, rectPage.bottom);
        m_listLogCategory.MoveWindow(&rectLogCategory);

        CRect rectLogSplitter(rectPage.left + m_nCategoryListWidth, rectPage.top,
            rectPage.left + m_nCategoryListWidth + 4, rectPage.bottom);
        m_logSplitter.MoveWindow(&rectLogSplitter);

        // 计算右边列表的布局，确保至少有50像素宽度且不超过页面边界
        int rightListLeft = rectPage.left + m_nCategoryListWidth + 4 + 6;
        int rightListRight = rectPage.right;

        // 如果计算出的右边距小于左边距，说明布局有问题，使用安全的默认值
        if (rightListRight <= rightListLeft + 50)
        {
            rightListRight = rightListLeft + max(50, rectPage.Width() - m_nCategoryListWidth - 10);
        }

        CRect rectLogLibrary(rightListLeft, rectPage.top, rightListRight, rectPage.bottom);
        m_listLogLibrary.MoveWindow(&rectLogLibrary);

        // 重新调整列宽
        if (m_listCategory.GetSafeHwnd())
        {
            m_listCategory.SetColumnWidth(0, m_nCategoryListWidth - 20);
        }
        if (m_listLogCategory.GetSafeHwnd())
        {
            m_listLogCategory.SetColumnWidth(0, m_nCategoryListWidth - 20);
        }
        if (m_listLogLibrary.GetSafeHwnd())
        {
            // 确保列宽至少为50像素
            int columnWidth = rectPage.Width() - m_nCategoryListWidth - 30;
            columnWidth = max(columnWidth, 50);
            m_listLogLibrary.SetColumnWidth(0, columnWidth);
        }
    }
}

// 鼠标左键按下事件（用于拖动分割条）
void CWDToolBoxDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect rectSplitter;
    // 检查工具管理器分页的分割条
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

    // 检查工作日志分页的分割条
    if (m_logSplitter.GetSafeHwnd())
    {
        m_logSplitter.GetWindowRect(&rectSplitter);
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
        // 检查鼠标是否还在窗口内，如果不在了则结束拖拽
        CRect clientRect;
        GetClientRect(&clientRect);
        if (!clientRect.PtInRect(point))
        {
            // 鼠标离开窗口，结束拖拽
            m_bDragging = FALSE;
            ReleaseCapture();

            // 拖拽结束后强制重绘，确保所有控件正确显示
            Invalidate();
            return;
        }

        // 计算新的宽度
        int nDeltaX = point.x - m_nDragStartX;
        int nNewWidth = m_nDragStartWidth + nDeltaX;

        // 限制宽度范围（最小150，为工作日志分页预留至少200像素宽度）
        if (nNewWidth < 150)
            nNewWidth = 150;
        // 预留至少200像素给工作日志分页的右边列表
        int maxWidth = min(clientRect.Width() / 2, clientRect.Width() - 200);
        if (nNewWidth > maxWidth)
            nNewWidth = maxWidth;

        // 更新宽度并重新布局（只有当宽度变化超过5像素时才更新，避免过度闪烁）
        if (abs(nNewWidth - m_nCategoryListWidth) >= 5)
        {
            m_nCategoryListWidth = nNewWidth;
            OnSize(0, clientRect.Width(), clientRect.Height());
        }
    }
    else
    {
        // 检查鼠标是否在分割条上，改变光标
        CRect rectSplitter;
        bool cursorSet = false;

        // 检查工具管理器分页的分割条
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.GetWindowRect(&rectSplitter);
            ScreenToClient(&rectSplitter);
            rectSplitter.InflateRect(5, 0);

            if (rectSplitter.PtInRect(point))
            {
                ::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));  // 左右调整光标
                cursorSet = true;
            }
        }

        // 检查工作日志分页的分割条
        if (!cursorSet && m_logSplitter.GetSafeHwnd())
        {
            m_logSplitter.GetWindowRect(&rectSplitter);
            ScreenToClient(&rectSplitter);
            rectSplitter.InflateRect(5, 0);

            if (rectSplitter.PtInRect(point))
            {
                ::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));  // 左右调整光标
                cursorSet = true;
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

        // 拖拽结束后强制重绘，确保所有控件正确显示
        Invalidate();
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

// 鼠标捕获改变事件（处理窗口大小调整时的鼠标捕获丢失）
void CWDToolBoxDlg::OnCaptureChanged(CWnd* pWnd)
{
    // 注意：这里主要处理窗口大小调整时的鼠标捕获丢失
    // 分割条拖拽的鼠标捕获丢失在 OnMouseMove 中处理
    // 这里不处理 m_bDragging 标志，因为那是分割条拖拽的状态
    m_bDragging = FALSE;
    ReleaseCapture();  // 释放鼠标捕获

    // 拖拽结束后强制重绘，确保所有控件正确显示
    Invalidate();
    CDialogEx::OnCaptureChanged(pWnd);
}

// 设置光标（在分割条上显示调整光标）
BOOL CWDToolBoxDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (pWnd == &m_splitter || pWnd->GetDlgCtrlID() == IDC_SPLITTER ||
        pWnd == &m_logSplitter || pWnd->GetDlgCtrlID() == IDC_LOG_SPLITTER)
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
    m_listLogCategory.ShowWindow(SW_HIDE);
    m_listLogLibrary.ShowWindow(SW_HIDE);
    m_logSplitter.ShowWindow(SW_HIDE);

    switch (nPage)
    {
    case 0: // 工具管理器分页
        m_listCategory.ShowWindow(SW_SHOW);
        m_listTool.ShowWindow(SW_SHOW);
        m_splitter.ShowWindow(SW_SHOW);
        break;
    case 1: // 工作日志分页
        m_listLogCategory.ShowWindow(SW_SHOW);
        m_listLogLibrary.ShowWindow(SW_SHOW);
        m_logSplitter.ShowWindow(SW_SHOW);
        LoadLogCategories();
        break;
    }
}

// 加载日志分类
void CWDToolBoxDlg::LoadLogCategories()
{
    // 先清空现有数据，避免重复添加
    m_workLogger.Clear();
    m_listLogCategory.DeleteAllItems();
    m_listLogLibrary.DeleteAllItems();

    // 从配置文件加载日志库，如果失败则使用默认配置
    if (!m_workLogger.LoadFromConfig(_T("")))
    {
        // 配置文件不存在或加载失败，将使用默认配置
        MessageBox(_T("日志配置文件不存在或加载失败，将使用默认配置。"), _T("提示"), MB_OK | MB_ICONINFORMATION);
        m_workLogger.LoadDefaultLibraries();
    }

    // 填充分类列表
    std::vector<CString> categories;
    m_workLogger.GetAllCategories(categories);
    for (size_t i = 0; i < categories.size(); i++)
    {
        int nIndex = m_listLogCategory.InsertItem((int)i, categories[i]);
        m_listLogCategory.SetItemData(nIndex, i);
    }

    // 默认选择第一项
    if (m_listLogCategory.GetItemCount() > 0)
    {
        m_listLogCategory.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        UpdateLogLibraryList(categories[0]);
    }
}

// 更新日志库列表
void CWDToolBoxDlg::UpdateLogLibraryList(const CString& strCategory)
{
    m_listLogLibrary.DeleteAllItems();

    std::vector<LogLibraryInfo>& libraries = m_workLogger.GetLibrariesByCategory(strCategory);
    for (size_t i = 0; i < libraries.size(); i++)
    {
        m_listLogLibrary.InsertItem((int)i, libraries[i].strName);
    }
}

// 日志分类列表选择改变事件
void CWDToolBoxDlg::OnLvnItemchangedLogCategoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (pNMLV->uChanged & LVIF_STATE && pNMLV->uNewState & LVIS_SELECTED)
    {
        CString strCategory = m_listLogCategory.GetItemText(pNMLV->iItem, 0);
        UpdateLogLibraryList(strCategory);
    }

    *pResult = 0;
}

// 日志库列表双击事件
void CWDToolBoxDlg::OnNMDblclkLogLibraryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    if (pNMItemActivate->iItem >= 0)
    {
        // 这里可以添加打开日志库的操作
		CString strLibraryInfo = m_listLogLibrary.GetItemText(pNMItemActivate->iItem, 0);
		CString strProjectInfo = m_listLogCategory.GetItemText(pNMItemActivate->iItem, 0);
		CWriteWorkLogDlg dlg(this);
		dlg.SetProjectInfo(strProjectInfo);
		dlg.SetLibraryInfo(strLibraryInfo);
		if (dlg.DoModal() == IDOK)
		{
			CString strLogContent = dlg.GetLogContent();
			// 只有当日志内容不为空时才写入
			if (!strLogContent.IsEmpty())
			{
				m_workLogWriter.Execute(strLogContent);
			}
		}
    }

    *pResult = 0;
}