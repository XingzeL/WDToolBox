#include "EditPromptFieldsDialog.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

CEditPromptFieldsDialog::CEditPromptFieldsDialog(QWidget* parent)
    : QDialog(parent)
    , m_editRole(nullptr)
    , m_editContext(nullptr)
    , m_editTask(nullptr)
    , m_editConstraints(nullptr)
    , m_editOutputFormat(nullptr)
    , m_editAcceptanceCriteria(nullptr)
{
    setWindowTitle(tr("Edit prompt fields"));
    setMinimumSize(520, 560);

    m_editRole = new QTextEdit(this);
    m_editContext = new QTextEdit(this);
    m_editTask = new QTextEdit(this);
    m_editConstraints = new QTextEdit(this);
    m_editOutputFormat = new QTextEdit(this);
    m_editAcceptanceCriteria = new QTextEdit(this);

    const int h = 72;
    m_editRole->setMinimumHeight(h);
    m_editContext->setMinimumHeight(h);
    m_editTask->setMinimumHeight(h);
    m_editConstraints->setMinimumHeight(h);
    m_editOutputFormat->setMinimumHeight(h);
    m_editAcceptanceCriteria->setMinimumHeight(h);

    QFormLayout* form = new QFormLayout();
    form->addRow(new QLabel(tr("Role")), m_editRole);
    form->addRow(new QLabel(tr("Context")), m_editContext);
    form->addRow(new QLabel(tr("Task")), m_editTask);
    form->addRow(new QLabel(tr("Constraints")), m_editConstraints);
    form->addRow(new QLabel(tr("Output Format")), m_editOutputFormat);
    form->addRow(new QLabel(tr("Acceptance Criteria")), m_editAcceptanceCriteria);

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addWidget(buttons);
}

void CEditPromptFieldsDialog::setPreset(const PromptRolePreset& preset)
{
    m_editRole->setPlainText(preset.role);
    m_editContext->setPlainText(preset.context);
    m_editTask->setPlainText(preset.task);
    m_editConstraints->setPlainText(preset.constraints);
    m_editOutputFormat->setPlainText(preset.outputFormat);
    m_editAcceptanceCriteria->setPlainText(preset.acceptanceCriteria);
}

PromptRolePreset CEditPromptFieldsDialog::fields() const
{
    PromptRolePreset p;
    p.role = m_editRole->toPlainText();
    p.context = m_editContext->toPlainText();
    p.task = m_editTask->toPlainText();
    p.constraints = m_editConstraints->toPlainText();
    p.outputFormat = m_editOutputFormat->toPlainText();
    p.acceptanceCriteria = m_editAcceptanceCriteria->toPlainText();
    return p;
}
