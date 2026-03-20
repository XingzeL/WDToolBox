#pragma once

#include "TabPageBase.h"
#include "PromptRolePreset.h"
#include <QListWidget>
#include <QSplitter>
#include <QHBoxLayout>

class QTextEdit;
class QPushButton;

// Third tab: structured prompt composer (left presets, right preview + copy)
class CPromptComposerPage : public TabPageBase
{
    Q_OBJECT

public:
    explicit CPromptComposerPage(QWidget* parent = nullptr);
    virtual ~CPromptComposerPage();

    virtual void updateLayout(int leftWidth) override;
    virtual int getLeftWidth() const override { return m_nLeftWidth; }
    virtual void setLeftWidth(int width) override { m_nLeftWidth = width; }

private slots:
    void onPresetItemClicked(QListWidgetItem* item);
    void onCopyClicked();

private:
    static QString buildFormattedPrompt(const PromptRolePreset& p);

    QListWidget* m_listPresets;
    QTextEdit*   m_textPreview;
    QPushButton* m_btnCopy;
    QSplitter*   m_splitter;
    int          m_nLeftWidth;
};
