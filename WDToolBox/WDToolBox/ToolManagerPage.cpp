// ToolManagerPage.cpp: 实现文件
//

#include "pch.h"
#include "ToolManagerPage.h"
#include "Resource.h"

CToolManagerPage::CToolManagerPage()
	: m_pParent(nullptr)
	, m_nLeftWidth(200)
	, m_bDragging(FALSE)
	, m_nDragStartX(0)
	, m_nDragStartWidth(0)
	, m_nMinWidth(150)
	, m_nMaxWidth(0)
{
}

CToolManagerPage::~CToolManagerPage()
{
}

void CToolManagerPage::CreateControls(CWnd* pParent, const CRect& rectPage)
{
	m_pParent = pParent;

	// 创建左侧分类列表
	CRect rectCategory(rectPage.left, rectPage.top,
		rectPage.left + m_nLeftWidth, rectPage.bottom);
	m_listCategory.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
		rectCategory, pParent, IDC_CATEGORY_LIST);
	m_listCategory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCategory.InsertColumn(0, _T("工具分类"), LVCFMT_LEFT, m_nLeftWidth - 20);

	// 创建分割条
	CRect rectSplitter(rectCategory.right, rectPage.top, rectCategory.right + 4, rectPage.bottom);
	m_splitter.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT,
		rectSplitter, pParent, IDC_SPLITTER);

	// 创建右侧工具列表
	CRect rectTool(rectSplitter.right + 6, rectPage.top, rectPage.right, rectPage.bottom);
	m_listTool.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_ICON | LVS_AUTOARRANGE,
		rectTool, pParent, IDC_TOOL_LIST);
	m_listTool.SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
}

void CToolManagerPage::UpdateLayout(const CRect& rectPage, int nLeftWidth)
{
	// 确保宽度在合理范围内
	int nAdjustedWidth = nLeftWidth;
	if (nAdjustedWidth < m_nMinWidth)
		nAdjustedWidth = m_nMinWidth;
	if (m_nMaxWidth > 0 && nAdjustedWidth > m_nMaxWidth)
		nAdjustedWidth = m_nMaxWidth;

	m_nLeftWidth = nAdjustedWidth;

	// 调整左侧列表
	CRect rectCategory(rectPage.left, rectPage.top,
		rectPage.left + nAdjustedWidth, rectPage.bottom);
	m_listCategory.MoveWindow(&rectCategory);

	// 调整分割条
	CRect rectSplitter(rectCategory.right, rectPage.top, rectCategory.right + 4, rectPage.bottom);
	m_splitter.MoveWindow(&rectSplitter);

	// 调整右侧列表
	CRect rectTool(rectSplitter.right + 6, rectPage.top, rectPage.right, rectPage.bottom);
	m_listTool.MoveWindow(&rectTool);

	// 重新调整列宽
	if (m_listCategory.GetSafeHwnd())
	{
		m_listCategory.SetColumnWidth(0, nAdjustedWidth - 20);
	}
}

void CToolManagerPage::Show()
{
	m_listCategory.ShowWindow(SW_SHOW);
	m_listTool.ShowWindow(SW_SHOW);
	m_splitter.ShowWindow(SW_SHOW);
}

void CToolManagerPage::Hide()
{
	m_listCategory.ShowWindow(SW_HIDE);
	m_listTool.ShowWindow(SW_HIDE);
	m_splitter.ShowWindow(SW_HIDE);
}

BOOL CToolManagerPage::OnLButtonDown(CPoint point, int& nCurrentWidth)
{
	if (IsPointOnSplitter(point))
	{
		m_bDragging = TRUE;
		m_nDragStartX = point.x;
		m_nDragStartWidth = nCurrentWidth;
		m_pParent->SetCapture();
		return TRUE;
	}
	return FALSE;
}

BOOL CToolManagerPage::OnMouseMove(CPoint point, int& nCurrentWidth, CRect& clientRect)
{
	if (m_bDragging)
	{
		int nDeltaX = point.x - m_nDragStartX;
		int nNewWidth = m_nDragStartWidth + nDeltaX;

		// 限制宽度范围
		if (nNewWidth < m_nMinWidth)
			nNewWidth = m_nMinWidth;
		if (m_nMaxWidth > 0 && nNewWidth > m_nMaxWidth)
			nNewWidth = m_nMaxWidth;

		// 检查是否超出窗口边界
		if (nNewWidth > clientRect.Width() - 200)
			nNewWidth = clientRect.Width() - 200;

		if (abs(nNewWidth - nCurrentWidth) >= 5)
		{
			nCurrentWidth = nNewWidth;
			m_nLeftWidth = nNewWidth;
			return TRUE; // 需要更新布局
		}
	}
	else if (IsPointOnSplitter(point))
	{
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		return TRUE;
	}
	return FALSE;
}

void CToolManagerPage::OnLButtonUp()
{
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		::ReleaseCapture();
	}
}

void CToolManagerPage::OnCaptureChanged()
{
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		::ReleaseCapture();
	}
}

BOOL CToolManagerPage::OnSetCursor(CPoint point)
{
	if (IsPointOnSplitter(point))
	{
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		return TRUE;
	}
	return FALSE;
}

BOOL CToolManagerPage::IsPointOnSplitter(CPoint point)
{
	if (!m_splitter.GetSafeHwnd())
		return FALSE;

	CRect rectSplitter;
	m_splitter.GetWindowRect(&rectSplitter);
	m_pParent->ScreenToClient(&rectSplitter);

	// 扩大可拖动区域（左右各扩展5像素）
	rectSplitter.InflateRect(5, 0);

	return rectSplitter.PtInRect(point);
}

