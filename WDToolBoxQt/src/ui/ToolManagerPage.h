#pragma once
#include "TabPageBase.h"
#include "../infrastructure/Observer.h"
#include "../core/ToolInfo.h"
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QContextMenuEvent>
#include <QMenu>
#include <QInputDialog>
#include <QColor>

// Forward declaration
class CToolManager;

// Tool Manager Page class
// Inherits from TabPageBase and IObserver, implements tool management interface
class CToolManagerPage : public TabPageBase, public IObserver
{
    Q_OBJECT

public:
    CToolManagerPage(QWidget* parent = nullptr);
    virtual ~CToolManagerPage();

    // TabPageBase interface implementation
    virtual void updateLayout(int leftWidth) override;
    virtual int getLeftWidth() const override { return m_nLeftWidth; }
    virtual void setLeftWidth(int width) override { m_nLeftWidth = width; }

    // IObserver interface implementation
    virtual void OnDataChanged(const QString& strEventType, void* pData = nullptr) override;

    // Get list widgets (for external access)
    QListWidget* GetCategoryList() { return m_listCategory; }
    QListWidget* GetToolList() { return m_listTool; }

    // Set ToolManager pointer (for observer pattern)
    void SetToolManager(CToolManager* pManager) { m_pToolManager = pManager; }

    // Refresh category list
    void RefreshCategoryList();

    // Refresh tool list (based on selected category)
    void RefreshToolList();

protected:
    // 拖拽相关事件
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onCategorySelectionChanged();
    void onToolDoubleClicked(QListWidgetItem* item);
    void onToolContextMenu(const QPoint& pos);
    void onRemoveTool();
    void onRenameTool();
    void onGetFullPath(QListWidgetItem* pItem);
    void onOpenInExplorer(QListWidgetItem* pItem);
    void onToggleHighlight(QListWidgetItem* pItem);

private:
    void applyHighlightVisual(QListWidgetItem* pItem, const ToolInfo& tool);

    // Member variables
    QListWidget* m_listCategory;    // Category list (left side)
    QListWidget* m_listTool;        // Tool list (right side, icon mode)
    QSplitter* m_splitter;          // Splitter

    CToolManager* m_pToolManager;   // Tool manager pointer (for observer pattern)
    int m_nLeftWidth;                // Left width (category list width)
};
