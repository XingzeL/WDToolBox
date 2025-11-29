
// WDToolBoxDlg.h: 头文件
//

#pragma once

#include "ToolInfo.h"
#include "ProcessLauncher.h"
#include "WorkLogManager.h"

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

	// 控件
	CListCtrl m_listCategory;    // 左侧分类列表
	CListCtrl m_listTool;        // 右侧工具图标列表
	CImageList m_imageList;      // 图标图像列表
	CStatic m_splitter;          // 分割条（用于调整左右控件宽度）

	// 管理器
	CToolManager m_toolManager;

	CProcessLauncher m_launcher;
	CWorkLogManager m_workLogger;
	
	// 布局参数
	int m_nCategoryListWidth;    // 左侧分类列表宽度（可调整）
	BOOL m_bDragging;             // 是否正在拖动分割条
	int m_nDragStartX;            // 拖动开始时的X坐标
	int m_nDragStartWidth;        // 拖动开始时的左侧列表宽度

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkToolList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	
	// 辅助函数
	void InitializeControls();
	void LoadToolCategories();
	void LoadToolsFromConfig(const CString& strConfigPath);
	void UpdateToolList(const CString& strCategory);

	DECLARE_MESSAGE_MAP()
};
