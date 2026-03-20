#include "PromptComposerPage.h"
#include "EditPromptFieldsDialog.h"
#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QVector>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QPushButton>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

static QVector<PromptRolePreset> defaultPresets()
{
    QVector<PromptRolePreset> v;

    // Custom: no preset text; user fills everything in the dialog
    PromptRolePreset custom;
    custom.displayName = QStringLiteral("Custom");
    custom.role.clear();
    custom.context.clear();
    custom.task.clear();
    custom.constraints.clear();
    custom.outputFormat.clear();
    custom.acceptanceCriteria.clear();
    v.append(custom);

    PromptRolePreset mfc;
    mfc.displayName = QString::fromUtf8("MFC expert");
    mfc.role = QString::fromUtf8(
        "You are a senior Windows desktop engineer specialized in MFC, Win32 API, "
        "resource editors, message maps, and legacy UI maintenance.");
    mfc.context = QString::fromUtf8(
        "The codebase is a Qt/MFC mixed or pure MFC application. Prefer minimal-risk "
        "changes compatible with existing patterns.");
    mfc.task.clear();
    mfc.constraints = QString::fromUtf8(
        "- Use MSVC-compatible C++.\n"
        "- Avoid breaking existing message maps and DDX/DDV.\n"
        "- If unsure, state assumptions explicitly.");
    mfc.outputFormat = QString::fromUtf8(
        "Structured answer with: Summary, Steps, Code snippets (if any), Risks, Tests.");
    mfc.acceptanceCriteria = QString::fromUtf8(
        "Solution compiles, matches MFC idioms, and lists any required resource/UI steps.");
    v.append(mfc);

    PromptRolePreset qt;
    qt.displayName = QString::fromUtf8("Qt expert");
    qt.role = QString::fromUtf8(
        "You are an expert in Qt 5 widgets, signals/slots, models/views, and deployment on Windows.");
    qt.context = QString::fromUtf8("Project uses Qt Widgets; prefer Qt APIs over platform-specific code when possible.");
    qt.task.clear();
    qt.constraints = QString::fromUtf8(
        "- Qt 5.12 compatible APIs.\n"
        "- Prefer RAII and parent-child ownership.\n"
        "- Mention threading rules if relevant.");
    qt.outputFormat = QString::fromUtf8(
        "Answer with: Plan, Code, Build notes, Edge cases.");
    qt.acceptanceCriteria = QString::fromUtf8(
        "Code is copy-paste friendly and builds with the stated kit.");
    v.append(qt);

    PromptRolePreset review;
    review.displayName = QString::fromUtf8("Code review");
    review.role = QString::fromUtf8("You are a careful code reviewer focused on correctness, security, and maintainability.");
    review.context = QString::fromUtf8("Review the provided change in the context of a small team C++ codebase.");
    review.task.clear();
    review.constraints = QString::fromUtf8(
        "- Be concise.\n"
        "- Separate must-fix vs nice-to-have.\n"
        "- No personal attacks.");
    review.outputFormat = QString::fromUtf8(
        "Sections: Findings (severity-tagged), Questions, Suggested patch outline.");
    review.acceptanceCriteria = QString::fromUtf8(
        "Each finding references file/area and rationale.");
    v.append(review);

    return v;
}

} // namespace

CPromptComposerPage::CPromptComposerPage(QWidget* parent)
    : TabPageBase(parent)
    , m_listPresets(nullptr)
    , m_textPreview(nullptr)
    , m_btnCopy(nullptr)
    , m_splitter(nullptr)
    , m_nLeftWidth(220)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(m_splitter);

    m_listPresets = new QListWidget(m_splitter);
    m_listPresets->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listPresets->setIconSize(QSize(24, 24));

    QWidget* right = new QWidget(m_splitter);
    QVBoxLayout* rightLay = new QVBoxLayout(right);
    rightLay->setContentsMargins(4, 4, 4, 4);
    m_textPreview = new QTextEdit(right);
    m_textPreview->setReadOnly(true);
    m_textPreview->setPlaceholderText(tr("Formatted prompt will appear here."));
    m_btnCopy = new QPushButton(tr("Copy"), right);
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(m_btnCopy);
    rightLay->addWidget(m_textPreview);
    rightLay->addLayout(btnRow);

    m_splitter->addWidget(m_listPresets);
    m_splitter->addWidget(right);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    QStyle* st = style();
    const QVector<PromptRolePreset> presets = defaultPresets();
    static const QStyle::StandardPixmap icons[] = {
        QStyle::SP_FileDialogNewFolder,
        QStyle::SP_ComputerIcon,
        QStyle::SP_DialogYesButton,
        QStyle::SP_FileDialogDetailedView,
    };
    for (int i = 0; i < presets.size(); ++i)
    {
        const PromptRolePreset& pr = presets[i];
        QIcon icon = st->standardIcon(icons[i % 4]);
        QListWidgetItem* it = new QListWidgetItem(icon, pr.displayName);
        it->setData(Qt::UserRole, i);
        m_listPresets->addItem(it);
    }

    connect(m_listPresets, &QListWidget::itemClicked, this, &CPromptComposerPage::onPresetItemClicked);
    connect(m_btnCopy, &QPushButton::clicked, this, &CPromptComposerPage::onCopyClicked);
}

CPromptComposerPage::~CPromptComposerPage()
{
}

void CPromptComposerPage::updateLayout(int leftWidth)
{
    int w = leftWidth;
    if (w < 160)
        w = 160;
    m_nLeftWidth = w;
    if (m_splitter)
    {
        QList<int> sizes;
        sizes << w << qMax(100, width() - w - m_splitter->handleWidth());
        m_splitter->setSizes(sizes);
    }
}

QString CPromptComposerPage::buildFormattedPrompt(const PromptRolePreset& p)
{
    QString out;
    out += QStringLiteral("## Role\n");
    out += p.role.trimmed();
    out += QStringLiteral("\n\n## Context\n");
    out += p.context.trimmed();
    out += QStringLiteral("\n\n## Task\n");
    out += p.task.trimmed();
    out += QStringLiteral("\n\n## Constraints\n");
    out += p.constraints.trimmed();
    out += QStringLiteral("\n\n## Output Format\n");
    out += p.outputFormat.trimmed();
    out += QStringLiteral("\n\n## Acceptance Criteria\n");
    out += p.acceptanceCriteria.trimmed();
    out += QLatin1Char('\n');
    return out;
}

void CPromptComposerPage::onPresetItemClicked(QListWidgetItem* item)
{
    if (!item)
        return;
    bool ok = false;
    int idx = item->data(Qt::UserRole).toInt(&ok);
    if (!ok)
        return;
    const QVector<PromptRolePreset> presets = defaultPresets();
    if (idx < 0 || idx >= presets.size())
        return;

    CEditPromptFieldsDialog dlg(this);
    dlg.setPreset(presets[idx]);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const PromptRolePreset filled = dlg.fields();
    m_textPreview->setPlainText(buildFormattedPrompt(filled));
}

void CPromptComposerPage::onCopyClicked()
{
    const QString t = m_textPreview->toPlainText();
    if (t.isEmpty())
        return;
    QClipboard* cb = QApplication::clipboard();
    if (cb)
        cb->setText(t);
}
