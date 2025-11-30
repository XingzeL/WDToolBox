// WDToolBoxDlg.h: 头文件
//

#pragma once

#include "ToolManager.h"
#include "WorkLogManager.h"
#include "TabPageBase.h"
#include "ToolManagerPage.h"
#include "WorkLogPage.h"
#include "ConfigReader.h"

// CWDToolBoxDlg 对话框
class CWDToolBoxDlg : public CDialogEx
{
// 构造
public:
	CWDToolBoxDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WDTOOLBOX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 分页控件
	CTabCtrl m_tabCtrl;          // 标签页控件

	// 分页类对象（每个分页封装自己的控件和逻辑）
	CToolManagerPage m_toolManagerPage;  // 工具管理器分页
	CWorkLogPage m_workLogPage;           // 工作日志分页

	CImageList m_imageList;      // 图标图像列表

	// 配置读取器（统一管理）
	CIniConfigReader m_toolConfigReader;    // 工具配置读取器
	CIniConfigReader m_logConfigReader;     // 日志配置读取器

	// 管理器（使用依赖注入，内部管理 Executor）
	CToolManager m_toolManager;
	CWorkLogManager m_workLogger;

	// 布局参数
	int m_nCategoryListWidth;    // 左侧分类列表宽度（可调整）

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkToolList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedLogCategoryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkLogLibraryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnTcnSelchangeTabMain(NMHDR* pNMHDR, LRESULT* pResult);

	// 辅助函数
	void InitializeControls();
	void LoadToolCategories();
	void UpdateToolList(const CString& strCategory);
	void LoadLogCategories();
	void UpdateLogLibraryList(const CString& strCategory);
	void ShowTabPage(int nPage);

	DECLARE_MESSAGE_MAP()
};
