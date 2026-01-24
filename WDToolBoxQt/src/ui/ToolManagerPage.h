#pragma once
#include "TabPageBase.h"
#include "../infrastructure/Observer.h"
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>

// ????
class CToolManager;

// ?????????????????
// ?????????????????????????
class CToolManagerPage : public TabPageBase, public IObserver
{
    Q_OBJECT

public:
    CToolManagerPage(QWidget* parent = nullptr);
    virtual ~CToolManagerPage();

    // TabPageBase ????
    virtual void updateLayout(int leftWidth) override;
    virtual int getLeftWidth() const override { return m_nLeftWidth; }
    virtual void setLeftWidth(int width) override { m_nLeftWidth = width; }

    // IObserver ????
    virtual void OnDataChanged(const QString& strEventType, void* pData = nullptr) override;

    // ?????????????
    QListWidget* GetCategoryList() { return m_listCategory; }
    QListWidget* GetToolList() { return m_listTool; }

    // ?? ToolManager ???????????
    void SetToolManager(CToolManager* pManager) { m_pToolManager = pManager; }

    // ??????
    void RefreshCategoryList();

    // ?????????????????
    void RefreshToolList();

private slots:
    void onCategorySelectionChanged();
    void onToolDoubleClicked(QListWidgetItem* item);

private:
    // ??
    QListWidget* m_listCategory;    // ??????
    QListWidget* m_listTool;        // ????????
    QSplitter* m_splitter;          // ???

    CToolManager* m_pToolManager;   // ????????????????
    int m_nLeftWidth;                // ??????
};
