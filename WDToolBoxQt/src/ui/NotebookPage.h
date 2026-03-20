#pragma once

#include "TabPageBase.h"
#include <QString>

class QTextEdit;

// Fourth tab: simple scratch notebook (paste / free text)
class CNotebookPage : public TabPageBase
{
    Q_OBJECT

public:
    explicit CNotebookPage(QWidget* parent = nullptr);
    virtual ~CNotebookPage();

    virtual void updateLayout(int leftWidth) override;
    virtual int getLeftWidth() const override { return m_nLeftWidth; }
    virtual void setLeftWidth(int width) override { m_nLeftWidth = width; }

    QString plainText() const;

    // Save current text to fixed folder; filename uses current local time. Returns path or empty on skip/failure.
    QString saveToTimestampedFile() const;

    static QString notebookSaveDirectory();

private slots:
    void onClearClicked();
    void onCopyAllClicked();
    void onSaveNowClicked();

private:
    QTextEdit* m_edit;
    int        m_nLeftWidth;
};
