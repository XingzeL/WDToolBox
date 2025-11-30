// WorkLogPage.cpp: 实现文件
//

#include "pch.h"
#include "WorkLogPage.h"
#include "Resource.h"

CWorkLogPage::CWorkLogPage()
	: m_pParent(nullptr)
	, m_nLeftWidth(200)
	, m_bDragging(FALSE)
	, m_nDragStartX(0)
	, m_nDragStartWidth(0)
	, m_nMinWidth(150)
	, m_nMaxWidth(0)
{
}

CWorkLogPage::~CWorkLogPage()
{
}

void CWorkLogPage::CreateControls(CWnd* pParent, const CRect& rectPage)
{
	m_pParent = pParent;

	// 创建左侧日志分类列表
	CRect rectLogCategory(rectPage.left, rectPage.top,
		rectPage.left + m_nLeftWidth, rectPage.bottom);
	m_listLogCategory.Create(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
		rectLogCategory, pParent, IDC_LOG_CATEGORY_LIST);
	m_listLogCategory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listLogCategory.InsertColumn(0, _T("日志分类"), LVCFMT_LEFT, m_nLeftWidth - 20);

	// 创建分割条
	CRect rectLogSplitter(rectPage.left + m_nLeftWidth, rectPage.top,
		rectPage.left + m_nLeftWidth + 4, rectPage.bottom);
	m_logSplitter.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_BLACKRECT,
		rectLogSplitter, pParent, IDC_LOG_SPLITTER);

	// 创建右侧日志库列表
	CRect rectLogLibrary(rectPage.left + m_nLeftWidth + 4 + 6, rectPage.top,
		rectPage.right, rectPage.bottom);
	m_listLogLibrary.Create(WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
		rectLogLibrary, pParent, IDC_LOG_LIBRARY_LIST);
	m_listLogLibrary.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listLogLibrary.InsertColumn(0, _T("Library Name"), LVCFMT_LEFT, rectPage.Width() - m_nLeftWidth - 30);
}

void CWorkLogPage::UpdateLayout(const CRect& rectPage, int nLeftWidth)
{
	// 确保宽度在合理范围内
	int nAdjustedWidth = nLeftWidth;
	if (nAdjustedWidth < m_nMinWidth)
		nAdjustedWidth = m_nMinWidth;
	if (m_nMaxWidth > 0 && nAdjustedWidth > m_nMaxWidth)
		nAdjustedWidth = m_nMaxWidth;

	m_nLeftWidth = nAdjustedWidth;

	// 调整左侧列表
	CRect rectLogCategory(rectPage.left, rectPage.top,
		rectPage.left + nAdjustedWidth, rectPage.bottom);
	m_listLogCategory.MoveWindow(&rectLogCategory);

	// 调整分割条
	CRect rectLogSplitter(rectPage.left + nAdjustedWidth, rectPage.top,
		rectPage.left + nAdjustedWidth + 4, rectPage.bottom);
	m_logSplitter.MoveWindow(&rectLogSplitter);

	// 计算右边列表的布局，确保至少有最小宽度且不超过页面边界
	int rightListLeft = rectPage.left + nAdjustedWidth + 4 + 6;
	int rightListRight = rectPage.right;

	// 如果计算出的右边距小于左边距，说明布局有问题，使用安全的默认值
	if (rightListRight <= rightListLeft + 50)
	{
		rightListRight = rightListLeft + max(50, rectPage.Width() - nAdjustedWidth - 10);
	}

	CRect rectLogLibrary(rightListLeft, rectPage.top, rightListRight, rectPage.bottom);
	m_listLogLibrary.MoveWindow(&rectLogLibrary);

	// 重新调整列宽
	if (m_listLogCategory.GetSafeHwnd())
	{
		m_listLogCategory.SetColumnWidth(0, nAdjustedWidth - 20);
	}
	if (m_listLogLibrary.GetSafeHwnd())
	{
		// 确保列宽至少为50像素
		int columnWidth = rectPage.Width() - nAdjustedWidth - 30;
		columnWidth = max(columnWidth, 50);
		m_listLogLibrary.SetColumnWidth(0, columnWidth);
	}
}

void CWorkLogPage::Show()
{
	m_listLogCategory.ShowWindow(SW_SHOW);
	m_listLogLibrary.ShowWindow(SW_SHOW);
	m_logSplitter.ShowWindow(SW_SHOW);
}

void CWorkLogPage::Hide()
{
	m_listLogCategory.ShowWindow(SW_HIDE);
	m_listLogLibrary.ShowWindow(SW_HIDE);
	m_logSplitter.ShowWindow(SW_HIDE);
}

BOOL CWorkLogPage::OnLButtonDown(CPoint point, int& nCurrentWidth)
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

BOOL CWorkLogPage::OnMouseMove(CPoint point, int& nCurrentWidth, CRect& clientRect)
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

void CWorkLogPage::OnLButtonUp()
{
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		::ReleaseCapture();
	}
}

void CWorkLogPage::OnCaptureChanged()
{
	if (m_bDragging)
	{
		m_bDragging = FALSE;
		::ReleaseCapture();
	}
}

BOOL CWorkLogPage::OnSetCursor(CPoint point)
{
	if (IsPointOnSplitter(point))
	{
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		return TRUE;
	}
	return FALSE;
}

BOOL CWorkLogPage::IsPointOnSplitter(CPoint point)
{
	if (!m_logSplitter.GetSafeHwnd())
		return FALSE;

	CRect rectSplitter;
	m_logSplitter.GetWindowRect(&rectSplitter);
	m_pParent->ScreenToClient(&rectSplitter);

	// 扩大可拖动区域（左右各扩展5像素）
	rectSplitter.InflateRect(5, 0);

	return rectSplitter.PtInRect(point);
}

