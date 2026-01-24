// CWriteWorkLogDialog.cpp: 实现文件
//
#pragma execution_character_set("utf-8")
#include "WriteWorkLogDialog.h"
#include <QDateTime>
#include <QMessageBox>

CWriteWorkLogDialog::CWriteWorkLogDialog(QWidget* parent)
    : QDialog(parent)
    , m_editTaskType(nullptr)
    , m_editLibName(nullptr)
    , m_comboTaskNature(nullptr)
    , m_comboTimeUsed(nullptr)
    , m_editProjectName(nullptr)
    , m_editProblemReason(nullptr)
    , m_editWorkDescribe(nullptr)
    , m_editName(nullptr)
    , m_comboManager(nullptr)
    , m_comboSystem(nullptr)
{
    setWindowTitle("写入工作日志");
    resize(600, 500);

    QVBoxLayout* pMainLayout = new QVBoxLayout(this);

    // 任务类型（只读）
    QHBoxLayout* pTaskTypeLayout = new QHBoxLayout();
    pTaskTypeLayout->addWidget(new QLabel("任务类型:", this));
    m_editTaskType = new QLineEdit(this);
    m_editTaskType->setReadOnly(true);
    pTaskTypeLayout->addWidget(m_editTaskType);
    pMainLayout->addLayout(pTaskTypeLayout);

    // 修改库名（只读）
    QHBoxLayout* pLibNameLayout = new QHBoxLayout();
    pLibNameLayout->addWidget(new QLabel("修改库名:", this));
    m_editLibName = new QLineEdit(this);
    m_editLibName->setReadOnly(true);
    pLibNameLayout->addWidget(m_editLibName);
    pMainLayout->addLayout(pLibNameLayout);

    // 姓名（只读，固定为李星泽）
    QHBoxLayout* pNameLayout = new QHBoxLayout();
    pNameLayout->addWidget(new QLabel("姓名:", this));
    m_editName = new QLineEdit(this);
    m_editName->setText("李星泽");
    m_editName->setReadOnly(true);
    pNameLayout->addWidget(m_editName);
    pMainLayout->addLayout(pNameLayout);

    // 项目名
    QHBoxLayout* pProjectNameLayout = new QHBoxLayout();
    pProjectNameLayout->addWidget(new QLabel("项目名:", this));
    m_editProjectName = new QLineEdit(this);
    pProjectNameLayout->addWidget(m_editProjectName);
    pMainLayout->addLayout(pProjectNameLayout);

    // 负责人下拉框
    QHBoxLayout* pManagerLayout = new QHBoxLayout();
    pManagerLayout->addWidget(new QLabel("负责人:", this));
    m_comboManager = new QComboBox(this);
    m_comboManager->addItem("刘跃雷");
    m_comboManager->addItem("张伟");
    m_comboManager->addItem("李曜");
    m_comboManager->addItem("唐旭");
    m_comboManager->addItem("李勃");
    m_comboManager->addItem("刘亚雷");
    m_comboManager->setCurrentIndex(0);
    pManagerLayout->addWidget(m_comboManager);
    pMainLayout->addLayout(pManagerLayout);

    // 系统下拉框
    QHBoxLayout* pSystemLayout = new QHBoxLayout();
    pSystemLayout->addWidget(new QLabel("系统:", this));
    m_comboSystem = new QComboBox(this);
    m_comboSystem->addItem("Windows");
    m_comboSystem->addItem("macOS");
    m_comboSystem->addItem("信创");
    m_comboSystem->addItem("Linux");
    m_comboSystem->addItem("Android");
    m_comboSystem->addItem("iOS");
    m_comboSystem->addItem("鸿蒙手机");
    m_comboSystem->addItem("鸿蒙PC");
    m_comboSystem->setCurrentIndex(0);
    pSystemLayout->addWidget(m_comboSystem);
    pMainLayout->addLayout(pSystemLayout);

    // 任务性质下拉框
    QHBoxLayout* pTaskNatureLayout = new QHBoxLayout();
    pTaskNatureLayout->addWidget(new QLabel("任务性质:", this));
    m_comboTaskNature = new QComboBox(this);
    m_comboTaskNature->addItem("需求变更开发");
    m_comboTaskNature->addItem("售后维护开发");
    m_comboTaskNature->addItem("新项目开发");
    m_comboTaskNature->addItem("标准产品开发");
    m_comboTaskNature->addItem("改善质量问题");
    m_comboTaskNature->addItem("客户提需求");
    m_comboTaskNature->addItem("其它");
    m_comboTaskNature->setCurrentIndex(0);
    pTaskNatureLayout->addWidget(m_comboTaskNature);
    pMainLayout->addLayout(pTaskNatureLayout);

    // 用时下拉框（0.5~8小时，0.5小时为单位）
    QHBoxLayout* pTimeUsedLayout = new QHBoxLayout();
    pTimeUsedLayout->addWidget(new QLabel("用时:", this));
    m_comboTimeUsed = new QComboBox(this);
    for (double dHour = 0.5; dHour <= 8.0; dHour += 0.5)
    {
        QString strTime;
        if (dHour == (int)dHour)
        {
            strTime = QString("%1").arg((int)dHour);
        }
        else
        {
            strTime = QString("%1").arg(dHour, 0, 'f', 1);
        }
        m_comboTimeUsed->addItem(strTime);
    }
    m_comboTimeUsed->setCurrentIndex(0);
    pTimeUsedLayout->addWidget(m_comboTimeUsed);
    pMainLayout->addLayout(pTimeUsedLayout);

    // 问题原因
    QHBoxLayout* pProblemReasonLayout = new QHBoxLayout();
    pProblemReasonLayout->addWidget(new QLabel("问题原因:", this));
    m_editProblemReason = new QLineEdit(this);
    pProblemReasonLayout->addWidget(m_editProblemReason);
    pMainLayout->addLayout(pProblemReasonLayout);

    // 工作描述
    QHBoxLayout* pWorkDescribeLayout = new QHBoxLayout();
    pWorkDescribeLayout->addWidget(new QLabel("工作描述:", this));
    m_editWorkDescribe = new QTextEdit(this);
    m_editWorkDescribe->setMaximumHeight(100);
    pWorkDescribeLayout->addWidget(m_editWorkDescribe);
    pMainLayout->addLayout(pWorkDescribeLayout);

    // 按钮
    QHBoxLayout* pButtonLayout = new QHBoxLayout();
    QPushButton* pOkBtn = new QPushButton("确定", this);
    QPushButton* pCancelBtn = new QPushButton("取消", this);
    pButtonLayout->addStretch();
    pButtonLayout->addWidget(pOkBtn);
    pButtonLayout->addWidget(pCancelBtn);
    pMainLayout->addLayout(pButtonLayout);

    connect(pOkBtn, &QPushButton::clicked, this, &CWriteWorkLogDialog::onOK);
    connect(pCancelBtn, &QPushButton::clicked, this, &CWriteWorkLogDialog::onCancel);
}

CWriteWorkLogDialog::~CWriteWorkLogDialog()
{
}

void CWriteWorkLogDialog::SetProjectInfo(const QString& strProjectInfo)
{
    m_strProjectInfo = strProjectInfo;
    if (m_editTaskType)
    {
        m_editTaskType->setText(strProjectInfo);
    }
}

void CWriteWorkLogDialog::SetLibraryInfo(const QString& strLibraryInfo)
{
    m_strLibraryInfo = strLibraryInfo;
    if (m_editLibName)
    {
        m_editLibName->setText(strLibraryInfo);
    }
}

QString CWriteWorkLogDialog::GetLogContent()
{
    return m_strLogContent;
}

QString CWriteWorkLogDialog::GetLogContenth()
{
    return m_strLogContenth;
}

// 辅助函数：替换字符串中的换行符为 " ;"
void CWriteWorkLogDialog::ReplaceNewlines(QString& str)
{
    str.replace("\r\n", " ;");
    str.replace("\n", " ;");
    str.replace("\r", " ;");
}

void CWriteWorkLogDialog::onOK()
{
    // 获取当前日期（不包含时分）
    QDateTime dateTime = QDateTime::currentDateTime();
    QString strDate = dateTime.toString("yyyy/MM/dd");

    // 获取任务类型（已设置，只读）
    QString strTaskType = m_strProjectInfo;

    // 获取修改库名（已设置，只读）
    QString strLibName = m_strLibraryInfo;

    // 获取任务性质
    QString strTaskNature = m_comboTaskNature->currentText();

    // 获取用时
    QString strTimeUsed = m_comboTimeUsed->currentText();

    // 获取新增字段
    QString strProjectName = m_editProjectName->text();      // 项目名
    QString strProblemReason = m_editProblemReason->text(); // 问题原因
    QString strWorkDescribe = m_editWorkDescribe->toPlainText(); // 工作描述（新）

    // 获取新增字段
    QString strName = m_editName->text();              // 姓名（固定为李星泽）
    QString strManager = m_comboManager->currentText(); // 负责人
    QString strSystem = m_comboSystem->currentText();   // 系统

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
    // 顺序：日期、姓名、项目名称、负责人、任务性质、系统、库名、工作描述、用时
    QString strTab = "\t";
    QString strReadableLog;

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
    QString strStructuredLog;
    strStructuredLog = "DT:" + strDate + strTab
        + "PJ:" + strProjectName + strTab
        + "TT:" + strTaskType + strTab
        + "LN:" + strLibName + strTab
        + "NT:" + strTaskNature + strTab
        + "TU:" + strTimeUsed + strTab
        + "PR:" + strProblemReason + strTab
        + "DS:" + strWorkDescribe + strTab
        + "NM:" + strName + strTab
        + "MG:" + strManager + strTab
        + "SY:" + strSystem;

    // 分离两个版本
    m_strLogContenth = strReadableLog;  // 人类可读版本
    m_strLogContent = strStructuredLog; // 结构化数据版本

    accept();
}

void CWriteWorkLogDialog::onCancel()
{
    // 取消时清空日志内容
    m_strLogContent.clear();
    m_strLogContenth.clear();
    reject();
}
