#include "NotebookPage.h"
#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

namespace {

QString ensureNotebookDir()
{
    const QString dir = CNotebookPage::notebookSaveDirectory();
    QDir d;
    if (!d.mkpath(dir))
        return QString();
    return dir;
}

} // namespace

QString CNotebookPage::notebookSaveDirectory()
{
    // Fixed location under application data (per user / per install)
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty())
        base = QCoreApplication::applicationDirPath();
    return QDir(base).filePath(QStringLiteral("notebook"));
}

CNotebookPage::CNotebookPage(QWidget* parent)
    : TabPageBase(parent)
    , m_edit(nullptr)
    , m_nLeftWidth(0)
{
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(4, 4, 4, 4);
    m_edit = new QTextEdit(this);
    m_edit->setAcceptRichText(false);
    m_edit->setPlaceholderText(tr(
        "Paste or type notes here. Content is saved when you close the app; you can also use the buttons below."));

    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* btnClear = new QPushButton(tr("Clear"), this);
    QPushButton* btnCopyAll = new QPushButton(tr("Copy all"), this);
    QPushButton* btnSaveNow = new QPushButton(tr("Save now"), this);
    btnRow->addWidget(btnClear);
    btnRow->addWidget(btnCopyAll);
    btnRow->addWidget(btnSaveNow);
    btnRow->addStretch();

    connect(btnClear, &QPushButton::clicked, this, &CNotebookPage::onClearClicked);
    connect(btnCopyAll, &QPushButton::clicked, this, &CNotebookPage::onCopyAllClicked);
    connect(btnSaveNow, &QPushButton::clicked, this, &CNotebookPage::onSaveNowClicked);

    lay->addWidget(m_edit);
    lay->addLayout(btnRow);
}

CNotebookPage::~CNotebookPage()
{
}

void CNotebookPage::updateLayout(int leftWidth)
{
    Q_UNUSED(leftWidth);
    m_nLeftWidth = 0;
}

QString CNotebookPage::plainText() const
{
    return m_edit ? m_edit->toPlainText() : QString();
}

QString CNotebookPage::saveToTimestampedFile() const
{
    const QString text = plainText().trimmed();
    if (text.isEmpty())
        return QString();

    const QString dir = ensureNotebookDir();
    if (dir.isEmpty())
        return QString();

    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"));
    const QString fileName = QStringLiteral("notebook_%1.txt").arg(stamp);
    const QString fullPath = QDir(dir).filePath(fileName);

    QFile f(fullPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return QString();

    QTextStream out(&f);
    out.setCodec("UTF-8");
    out << text;
    f.close();
    return fullPath;
}

void CNotebookPage::onClearClicked()
{
    if (!m_edit || m_edit->toPlainText().isEmpty())
        return;

    const int ret = QMessageBox::question(
        this,
        tr("Clear"),
        tr("Clear all content in the notebook?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (ret == QMessageBox::Yes)
        m_edit->clear();
}

void CNotebookPage::onCopyAllClicked()
{
    if (!m_edit)
        return;
    QClipboard* cb = QApplication::clipboard();
    if (cb)
        cb->setText(m_edit->toPlainText());
}

void CNotebookPage::onSaveNowClicked()
{
    const QString path = saveToTimestampedFile();
    if (path.isEmpty())
    {
        QMessageBox::information(
            this,
            tr("Save"),
            tr("Nothing to save, or failed to write the file."));
    }
    else
    {
        QMessageBox::information(
            this,
            tr("Saved"),
            tr("Saved to:\n%1").arg(path));
    }
}
