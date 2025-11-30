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
	DDX_Control(pDX, IDC_TASKTYPE, m_editTaskType);
	DDX_Control(pDX, IDC_EDITLIBNAME, m_editLibName);
	DDX_Control(pDX, IDC_TASKNATURE, m_comboTaskNature);
	DDX_Control(pDX, IDC_TIMEUSED, m_comboTimeUsed);
	DDX_Control(pDX, IDC_WORKDESC, m_editWorkDesc);
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
	ON_BN_CLICKED(IDOK, &CWriteWorkLogDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CWriteWorkLogDlg::OnCancel)
END_MESSAGE_MAP()


// CWriteWorkLogDlg 消息处理程序

BOOL CWriteWorkLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置任务类型和修改库名（只读）
	m_editTaskType.SetWindowText(m_strProjectInfo);
	m_editTaskType.SetReadOnly(TRUE);
	m_editLibName.SetWindowText(m_strLibraryInfo);
	m_editLibName.SetReadOnly(TRUE);

	// 初始化任务性质下拉框
	m_comboTaskNature.AddString(_T("开发"));
	m_comboTaskNature.AddString(_T("测试"));
	m_comboTaskNature.AddString(_T("修复"));
	m_comboTaskNature.AddString(_T("优化"));
	m_comboTaskNature.AddString(_T("文档"));
	m_comboTaskNature.AddString(_T("其他"));
	m_comboTaskNature.SetCurSel(0); // 默认选择第一项

	// 初始化用时下拉框（0.5~8小时，0.5小时为单位）
	for (double dHour = 0.5; dHour <= 8.0; dHour += 0.5)
	{
		CString strTime;
		if (dHour == (int)dHour)
		{
			strTime.Format(_T("%.0f小时"), dHour);
		}
		else
		{
			strTime.Format(_T("%.1f小时"), dHour);
		}
		m_comboTimeUsed.AddString(strTime);
	}
	m_comboTimeUsed.SetCurSel(0); // 默认选择第一项（0.5小时）

	return TRUE;  // 返回 TRUE 除非将焦点设置到控件
}

void CWriteWorkLogDlg::OnOK()
{
	// 获取当前日期时间
	CTime time = CTime::GetCurrentTime();
	CString strDate;
	strDate.Format(_T("%04d/%02d/%02d"), time.GetYear(), time.GetMonth(), time.GetDay());

	// 获取任务类型（已设置，只读）
	CString strTaskType = m_strProjectInfo;

	// 获取修改库名（已设置，只读）
	CString strLibName = m_strLibraryInfo;

	// 获取任务性质
	CString strTaskNature;
	int nSel = m_comboTaskNature.GetCurSel();
	if (nSel >= 0)
	{
		m_comboTaskNature.GetLBText(nSel, strTaskNature);
	}

	// 获取用时
	CString strTimeUsed;
	nSel = m_comboTimeUsed.GetCurSel();
	if (nSel >= 0)
	{
		m_comboTimeUsed.GetLBText(nSel, strTimeUsed);
	}

	// 获取工作描述
	CString strWorkDesc;
	m_editWorkDesc.GetWindowText(strWorkDesc);

	// 组合成格式化日志条目：[年月日][任务类型][修改库名][任务性质][用时][工作描述]
	m_strLogContent.Format(_T("[%s][%s][%s][%s][%s][%s]"),
		strDate, strTaskType, strLibName, strTaskNature, strTimeUsed, strWorkDesc);

	CDialogEx::OnOK();
}

void CWriteWorkLogDlg::OnCancel()
{
	// 取消时清空日志内容
	m_strLogContent.Empty();
	CDialogEx::OnCancel();
}
