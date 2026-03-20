// MainWidget.h
#pragma once

#include <QWidget>
#include <QCloseEvent>
#include <QTabWidget>
#include <QVBoxLayout>
#include "../core/ToolManager.h"
#include "../core/WorkLogManager.h"
#include "../infrastructure/ConfigReader.h"
#include "../infrastructure/ConfigFileWatcher.h"
#include "ToolManagerPage.h"
#include "WorkLogPage.h"
#include "PromptComposerPage.h"
#include "NotebookPage.h"

// main window
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    virtual ~MainWidget();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void onTabChanged(int index);
    void onToolConfigFileChanged(const QString& path);
    void onLogConfigFileChanged(const QString& path);

private:
    void InitializeControls();
    void LoadToolCategories();
    void LoadLogCategories();
    void StartConfigFileWatcher();

    QTabWidget* m_tabCtrl;

    CToolManagerPage*   m_toolManagerPage;
    CWorkLogPage*       m_workLogPage;
    CPromptComposerPage* m_promptComposerPage;
    CNotebookPage*         m_notebookPage;

    CIniConfigReader* m_toolConfigReader;
    CIniConfigReader* m_logConfigReader;

    CToolManager*     m_toolManager;
    CWorkLogManager*  m_workLogger;

    CConfigFileWatcher* m_configWatcher;

    int m_nCategoryListWidth;
};
