#pragma once
#include "afxwin.h"

// 分页基类接口
// 定义所有分页的通用接口
class ITabPage
{
public:
	virtual ~ITabPage() {}

	// 创建分页控件
	virtual void CreateControls(CWnd* pParent, const CRect& rectPage) = 0;

	// 更新布局（窗口大小改变时调用）
	virtual void UpdateLayout(const CRect& rectPage, int nLeftWidth) = 0;

	// 显示分页
	virtual void Show() = 0;

	// 隐藏分页
	virtual void Hide() = 0;

	// 处理鼠标按下事件（用于分割条拖拽）
	virtual BOOL OnLButtonDown(CPoint point, int& nCurrentWidth) { return FALSE; }

	// 处理鼠标移动事件（用于分割条拖拽）
	virtual BOOL OnMouseMove(CPoint point, int& nCurrentWidth, CRect& clientRect) { return FALSE; }

	// 处理鼠标释放事件
	virtual void OnLButtonUp() {}

	// 处理鼠标捕获改变
	virtual void OnCaptureChanged() {}

	// 检查鼠标是否在分割条上，设置光标
	virtual BOOL OnSetCursor(CPoint point) { return FALSE; }

	// 获取当前左侧宽度
	virtual int GetLeftWidth() const { return 0; }

	// 设置左侧宽度
	virtual void SetLeftWidth(int nWidth) {}
};

