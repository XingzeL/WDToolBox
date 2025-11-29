// CWriteWorkLogDlg.cpp: 实现文件
//

#include "pch.h"
#include "WDToolBox.h"
#include "afxdialogex.h"
#include "WriteWorkLogDlg.h"


// CWriteWorkLogDlg 对话框

IMPLEMENT_DYNAMIC(CWriteWorkLogDlg, CDialogEx)

CWriteWorkLogDlg::CWriteWorkLogDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WRITEWORKLOG, pParent)
{
	m_strProjectInfo = _T("");
	m_strLibraryInfo = _T("");
}

CWriteWorkLogDlg::~CWriteWorkLogDlg()
{
}

void CWriteWorkLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CWriteWorkLogDlg::SetProjectInfo(const CString& strProjectInfo)
{
	m_strProjectInfo = strProjectInfo;
}

void CWriteWorkLogDlg::SetLibraryInfo(const CString& strLibraryInfo)
{
	m_strLibraryInfo = strLibraryInfo;
}

CString CWriteWorkLogDlg::GetLogContent()
{
	return m_strLogContent;
}

BEGIN_MESSAGE_MAP(CWriteWorkLogDlg, CDialogEx)
END_MESSAGE_MAP()


// CWriteWorkLogDlg 消息处理程序
