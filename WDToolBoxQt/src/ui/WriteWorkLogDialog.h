#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>

// CWriteWorkLogDialog 对话框

class CWriteWorkLogDialog : public QDialog
{
    Q_OBJECT

public:
    CWriteWorkLogDialog(QWidget* parent = nullptr);
    virtual ~CWriteWorkLogDialog();

    void SetProjectInfo(const QString& strProjectInfo);
    void SetLibraryInfo(const QString& strLibraryInfo);
    QString GetLogContent();
    QString GetLogContenth();  // 获取人类可读版本的日志

private slots:
    void onOK();
    void onCancel();

private:
    // 辅助函数：替换字符串中的换行符为 " ;"
    void ReplaceNewlines(QString& str);

    QString m_strProjectInfo;
    QString m_strLibraryInfo;
    QString m_strLogContent;      // 结构化数据版本（用于程序解析）
    QString m_strLogContenth;     // 人类可读版本（用于人类阅读）

    // 控件变量
    QLineEdit* m_editTaskType;      // 任务类型（只读显示）
    QLineEdit* m_editLibName;       // 修改库名（只读显示）
    QComboBox* m_comboTaskNature;   // 任务性质下拉框
    QComboBox* m_comboTimeUsed;      // 用时下拉框

    // 新增控件变量
    QLineEdit* m_editProjectName;    // 项目名
    QLineEdit* m_editProblemReason;  // 问题原因
    QTextEdit* m_editWorkDescribe;    // 工作描述（新）

    // 新增控件变量
    QLineEdit* m_editName;           // 姓名（固定为李星泽）
    QComboBox* m_comboManager;       // 负责人下拉框
    QComboBox* m_comboSystem;        // 系统下拉框
};
