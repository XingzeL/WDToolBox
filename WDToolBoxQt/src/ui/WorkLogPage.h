#pragma once
#include "TabPageBase.h"
#include "../infrastructure/Observer.h"
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>

// ????
class CWorkLogManager;

// ????????????????
// ????????????????????????
class CWorkLogPage : public TabPageBase, public IObserver
{
    Q_OBJECT

public:
    CWorkLogPage(QWidget* parent = nullptr);
    virtual ~CWorkLogPage();

    // TabPageBase ????
    virtual void updateLayout(int leftWidth) override;
    virtual int getLeftWidth() const override { return m_nLeftWidth; }
    virtual void setLeftWidth(int width) override { m_nLeftWidth = width; }

    // IObserver ????
    virtual void OnDataChanged(const QString& strEventType, void* pData = nullptr) override;

    // ?????????????
    QListWidget* GetCategoryList() { return m_listLogCategory; }
    QListWidget* GetLibraryList() { return m_listLogLibrary; }

    // ?? WorkLogManager ???????????
    void SetWorkLogManager(CWorkLogManager* pManager) { m_pWorkLogManager = pManager; }

    // ??????
    void RefreshCategoryList();

    // ??????????????????
    void RefreshLibraryList();

private slots:
    void onCategorySelectionChanged();
    void onLibraryDoubleClicked(QListWidgetItem* item);

private:
    // ??
    QListWidget* m_listLogCategory;  // ????????
    QListWidget* m_listLogLibrary;   // ???????
    QSplitter* m_splitter;           // ???

    CWorkLogManager* m_pWorkLogManager; // ??????????????????
    int m_nLeftWidth;                   // ??????
};
