#pragma once
#include "TabPageBase.h"
#include "afxcmn.h"

// 工作日志分页类
// 封装工作日志分页的所有逻辑：控件、布局、分割条等
class CWorkLogPage : public ITabPage
{
public:
	CWorkLogPage();
	virtual ~CWorkLogPage();

	// ITabPage 接口实现
	virtual void CreateControls(CWnd* pParent, const CRect& rectPage) override;
	virtual void UpdateLayout(const CRect& rectPage, int nLeftWidth) override;
	virtual void Show() override;
	virtual void Hide() override;
	virtual BOOL OnLButtonDown(CPoint point, int& nCurrentWidth) override;
	virtual BOOL OnMouseMove(CPoint point, int& nCurrentWidth, CRect& clientRect) override;
	virtual void OnLButtonUp() override;
	virtual void OnCaptureChanged() override;
	virtual BOOL OnSetCursor(CPoint point) override;
	virtual int GetLeftWidth() const override { return m_nLeftWidth; }
	virtual void SetLeftWidth(int nWidth) override { m_nLeftWidth = nWidth; }

	// 获取控件指针（供外部使用）
	CListCtrl* GetCategoryList() { return &m_listLogCategory; }
	CListCtrl* GetLibraryList() { return &m_listLogLibrary; }

private:
	// 控件
	CListCtrl m_listLogCategory; // 左侧日志分类列表
	CListCtrl m_listLogLibrary;  // 右侧日志库列表
	CStatic m_logSplitter;       // 分割条

	CWnd* m_pParent;             // 父窗口
	int m_nLeftWidth;            // 左侧列表宽度

	// 分割条拖拽相关
	BOOL m_bDragging;
	int m_nDragStartX;
	int m_nDragStartWidth;
	int m_nMinWidth;
	int m_nMaxWidth;

	// 检查点是否在分割条上
	BOOL IsPointOnSplitter(CPoint point);
};

