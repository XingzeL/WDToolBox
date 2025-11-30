#pragma once
#include "TabPageBase.h"
#include "Observer.h"
#include "afxcmn.h"

// 前向声明
class CToolManager;

// 工具管理器分页类（实现观察者模式）
// 封装工具管理器分页的所有逻辑：控件、布局、分割条等
class CToolManagerPage : public ITabPage, public IObserver
{
public:
	CToolManagerPage();
	virtual ~CToolManagerPage();

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

	// IObserver 接口实现
	virtual void OnDataChanged(const CString& strEventType, void* pData = nullptr) override;

	// 获取控件指针（供外部使用）
	CListCtrl* GetCategoryList() { return &m_listCategory; }
	CListCtrl* GetToolList() { return &m_listTool; }

	// 设置 ToolManager 指针（用于观察者模式）
	void SetToolManager(CToolManager* pManager) { m_pToolManager = pManager; }

	// 刷新分类列表
	void RefreshCategoryList();

	// 刷新工具列表（根据当前选中的分类）
	void RefreshToolList();

private:
	// 控件
	CListCtrl m_listCategory;    // 左侧分类列表
	CListCtrl m_listTool;        // 右侧工具图标列表
	CStatic m_splitter;          // 分割条

	CWnd* m_pParent;             // 父窗口
	CToolManager* m_pToolManager; // 工具管理器指针（用于观察者模式）
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

