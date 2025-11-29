#pragma once
#include "afxdialogex.h"


// CWriteWorkLogDlg 对话框

class CWriteWorkLogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWriteWorkLogDlg)

/*
		CString strLibraryInfo = m_listLogLibrary.GetItemText(pNMItemActivate->iItem, 0);
		CString strProjectInfo = m_listLogCategory.GetItemText(pNMItemActivate->iItem, 0);
		CWriteWorkLogDlg dlg(this);
		dlg.SetProjectInfo(strProjectInfo);
		dlg.SetLibraryInfo(strLibraryInfo);
		dlg.DoModal();	
		m_workLogWriter.Execute(dlg.GetLogContent());
*/

public:

	CWriteWorkLogDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CWriteWorkLogDlg();

	void SetProjectInfo(const CString& strProjectInfo);
	void SetLibraryInfo(const CString& strLibraryInfo);
	CString GetLogContent();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WRITEWORKLOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
private:
	CString m_strProjectInfo;
	CString m_strLibraryInfo;
	CString m_strLogContent;


};
