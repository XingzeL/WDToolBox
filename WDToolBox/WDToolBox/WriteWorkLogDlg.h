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
	CString GetLogContenth();  // 获取人类可读版本的日志

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WRITEWORKLOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	DECLARE_MESSAGE_MAP()
private:
	// 辅助函数：替换字符串中的换行符为 " ;"
	void ReplaceNewlines(CString& str);
	CString m_strProjectInfo;
	CString m_strLibraryInfo;
	CString m_strLogContent;      // 结构化数据版本（用于程序解析）
	CString m_strLogContenth;     // 人类可读版本（用于人类阅读）

	// 控件变量
	CEdit m_editTaskType;      // 任务类型（只读显示）
	CEdit m_editLibName;       // 修改库名（只读显示）
	CComboBox m_comboTaskNature; // 任务性质下拉框
	CComboBox m_comboTimeUsed;   // 用时下拉框
	//CEdit m_editWorkDesc;       // 工作描述编辑框

	// 新增控件变量
	CEdit m_editProjectName;    // 项目名
	CEdit m_editProblemReason;  // 问题原因
	CEdit m_editWorkDescribe;   // 工作描述（新）

	// 新增控件变量
	CEdit m_editName;            // 姓名（固定为李星泽）
	CComboBox m_comboManager;    // 负责人下拉框
	CComboBox m_comboSystem;     // 系统下拉框
};
