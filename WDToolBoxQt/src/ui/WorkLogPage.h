#pragma once
#include "TabPageBase.h"
#include "../infrastructure/Observer.h"
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>

// 前向声明
class CWorkLogManager;

// 工作日志页面类
// 继承自 TabPageBase 和 IObserver，实现日志管理界面
class CWorkLogPage : public TabPageBase, public IObserver
{
    Q_OBJECT

public:
    CWorkLogPage(QWidget* parent = nullptr);
    virtual ~CWorkLogPage();

    // TabPageBase 接口实现
    virtual void updateLayout(int leftWidth) override;
    virtual int getLeftWidth() const override { return m_nLeftWidth; }
    virtual void setLeftWidth(int width) override { m_nLeftWidth = width; }

    // IObserver 接口实现
    virtual void OnDataChanged(const QString& strEventType, void* pData = nullptr) override;

    // 获取列表控件（供外部访问）
    QListWidget* GetCategoryList() { return m_listLogCategory; }
    QListWidget* GetLibraryList() { return m_listLogLibrary; }

    // 设置 WorkLogManager 指针（用于观察者模式）
    void SetWorkLogManager(CWorkLogManager* pManager) { m_pWorkLogManager = pManager; }

    // 刷新分类列表
    void RefreshCategoryList();

    // 刷新日志库列表（根据当前选中分类）
    void RefreshLibraryList();

private slots:
    void onCategorySelectionChanged();
    void onLibraryDoubleClicked(QListWidgetItem* item);

private:
    // 成员变量
    QListWidget* m_listLogCategory;  // 日志分类列表（左侧）
    QListWidget* m_listLogLibrary;   // 日志库列表（右侧）
    QSplitter* m_splitter;           // 分割器

    CWorkLogManager* m_pWorkLogManager; // 日志管理器指针（观察者模式）
    int m_nLeftWidth;                   // 左侧宽度
};
