// CWriteWorkLogDlg.cpp: 实现文件
//

#include "pch.h"
#include "WDToolBox.h"
#include "afxdialogex.h"
#include "WriteWorkLogDlg.h"
#include "resource.h"


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
	//DDX_Control(pDX, IDC_WORKDESC, m_editWorkDesc);
	DDX_Control(pDX, IDC_PROJECTNAME, m_editProjectName);
	DDX_Control(pDX, IDC_PROBLEMREASON, m_editProblemReason);
	DDX_Control(pDX, IDC_WORKDESCRIBE, m_editWorkDescribe);
	DDX_Control(pDX, IDC_NAME, m_editName);
	DDX_Control(pDX, IDC_MANAGER, m_comboManager);
	DDX_Control(pDX, IDC_SYSTEM, m_comboSystem);
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

CString CWriteWorkLogDlg::GetLogContenth()
{
	return m_strLogContenth;
}

// 辅助函数：替换字符串中的换行符为 " ;"
void CWriteWorkLogDlg::ReplaceNewlines(CString& str)
{
	str.Replace(_T("\r\n"), _T(" ;"));
	str.Replace(_T("\n"), _T(" ;"));
	str.Replace(_T("\r"), _T(" ;"));
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

	// 设置姓名（固定为李星泽，只读）
	m_editName.SetWindowText(_T("李星泽"));
	m_editName.SetReadOnly(TRUE);

	// 初始化负责人下拉框
	m_comboManager.AddString(_T("刘跃雷"));
	m_comboManager.AddString(_T("张伟"));
	m_comboManager.AddString(_T("李耀"));
	m_comboManager.AddString(_T("唐旭"));
	m_comboManager.AddString(_T("李勃"));
	m_comboManager.AddString(_T("刘亚雷"));
	m_comboManager.SetCurSel(0); // 默认选择第一项

	// 初始化系统下拉框
	m_comboSystem.AddString(_T("Windows"));
	m_comboSystem.AddString(_T("macOS"));
	m_comboSystem.AddString(_T("信创"));
	m_comboSystem.AddString(_T("Linux"));
	m_comboSystem.AddString(_T("Android"));
	m_comboSystem.AddString(_T("iOS"));
	m_comboSystem.AddString(_T("鸿蒙手机"));
	m_comboSystem.AddString(_T("鸿蒙PC"));
	m_comboSystem.SetCurSel(0); // 默认选择第一项

	// 初始化任务性质下拉框
	m_comboTaskNature.AddString(_T("需求变更开发"));
	m_comboTaskNature.AddString(_T("售后维护开发"));
	m_comboTaskNature.AddString(_T("新项目开发"));
	m_comboTaskNature.AddString(_T("标准产品开发"));
	m_comboTaskNature.AddString(_T("改善质量问题"));
	m_comboTaskNature.AddString(_T("客户提需求"));
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
	// 获取当前日期（不包含时分）
	CTime time = CTime::GetCurrentTime();
	CString strDate;
	strDate.Format(_T("%04d/%02d/%02d"),
		time.GetYear(), time.GetMonth(), time.GetDay());

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

	//// 获取工作描述
	//CString strWorkDesc;
	//m_editWorkDesc.GetWindowText(strWorkDesc);

	// 获取新增字段
	CString strProjectName;      // 项目名
	m_editProjectName.GetWindowText(strProjectName);

	CString strProblemReason;    // 问题原因
	m_editProblemReason.GetWindowText(strProblemReason);

	CString strWorkDescribe;     // 工作描述（新）
	m_editWorkDescribe.GetWindowText(strWorkDescribe);

	// 获取新增字段
	CString strName;              // 姓名（固定为李星泽）
	m_editName.GetWindowText(strName);

	CString strManager;           // 负责人
	nSel = m_comboManager.GetCurSel();
	if (nSel >= 0)
	{
		m_comboManager.GetLBText(nSel, strManager);
	}

	CString strSystem;            // 系统
	nSel = m_comboSystem.GetCurSel();
	if (nSel >= 0)
	{
		m_comboSystem.GetLBText(nSel, strSystem);
	}

	// 替换所有字段中的换行符为 " ;"
	ReplaceNewlines(strProjectName);
	ReplaceNewlines(strLibName);
	ReplaceNewlines(strTaskType);
	ReplaceNewlines(strTaskNature);
	ReplaceNewlines(strTimeUsed);
	ReplaceNewlines(strProblemReason);
	ReplaceNewlines(strWorkDescribe);
	ReplaceNewlines(strName);
	ReplaceNewlines(strManager);
	ReplaceNewlines(strSystem);

	// 组合成格式化日志条目
	// 第一部分：人类可读的版本（logsh.txt），按照指定顺序用 tab 分隔
	// 顺序：日期、姓名、项目名称、任务类型、负责人、任务性质、系统、库名、工作描述、用时
	CString strTab = _T("\t");
	CString strReadableLog;

	// 按照指定顺序用 tab 分隔
	strReadableLog = strDate + strTab
		+ strName + strTab
		+ strProjectName + strTab
		+ strManager + strTab
		+ strTaskNature + strTab
		+ strSystem + strTab
		+ strLibName + strTab
		+ strWorkDescribe + strTab
		+ strTimeUsed;

	// 第二部分：结构化数据格式，用 tab 分隔各个字段，便于程序解析和统计
	// 格式：Header:Value 使用大写字母缩写：DT=日期, PJ=项目, TT=任务类型, LN=库名, NT=性质, TU=用时, PR=原因, DS=描述, NM=姓名, MG=负责人, SY=系统
	CString strStructuredLog;
	strStructuredLog = _T("DT:") + strDate + strTab
		+ _T("PJ:") + strProjectName + strTab
		+ _T("TT:") + strTaskType + strTab
		+ _T("LN:") + strLibName + strTab
		+ _T("NT:") + strTaskNature + strTab
		+ _T("TU:") + strTimeUsed + strTab
		+ _T("PR:") + strProblemReason + strTab
		+ _T("DS:") + strWorkDescribe + strTab
		+ _T("NM:") + strName + strTab
		+ _T("MG:") + strManager + strTab
		+ _T("SY:") + strSystem;

	// 分离两个版本
	m_strLogContenth = strReadableLog;  // 人类可读版本
	m_strLogContent = strStructuredLog; // 结构化数据版本

	CDialogEx::OnOK();
}

void CWriteWorkLogDlg::OnCancel()
{
	// 取消时清空日志内容
	m_strLogContent.Empty();
	m_strLogContenth.Empty();
	CDialogEx::OnCancel();
}
