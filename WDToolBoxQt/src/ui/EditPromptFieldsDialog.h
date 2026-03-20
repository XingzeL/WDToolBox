#pragma once

#include "PromptRolePreset.h"
#include <QDialog>

class QTextEdit;

// Dialog: Role, Context, Task, Constraints, Output Format, Acceptance Criteria
class CEditPromptFieldsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CEditPromptFieldsDialog(QWidget* parent = nullptr);

    void setPreset(const PromptRolePreset& preset);

    PromptRolePreset fields() const;
    bool saveToConfigRequested() const { return m_bSaveToConfigRequested; }

private slots:
    void onSaveToConfigClicked();

private:
    QTextEdit* m_editRole;
    QTextEdit* m_editContext;
    QTextEdit* m_editTask;
    QTextEdit* m_editConstraints;
    QTextEdit* m_editOutputFormat;
    QTextEdit* m_editAcceptanceCriteria;
    bool       m_bSaveToConfigRequested;
};
