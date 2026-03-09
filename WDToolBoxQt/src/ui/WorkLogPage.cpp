// WorkLogPage.cpp: 工作日志页面实现
//
#include "WorkLogPage.h"
#include "../core/WorkLogManager.h"
#include "WriteWorkLogDialog.h"
#include "../infrastructure/WorkLogWriter.h"
#include <QListWidgetItem>
#include <QDialog>
#include <QDebug>

CWorkLogPage::CWorkLogPage(QWidget* parent)
    : TabPageBase(parent)
    , m_listLogCategory(nullptr)
    , m_listLogLibrary(nullptr)
    , m_splitter(nullptr)
    , m_pWorkLogManager(nullptr)
    , m_nLeftWidth(200)
{
    // 创建主布局
    QHBoxLayout* pMainLayout = new QHBoxLayout(this);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);

    // 创建分割器
    m_splitter = new QSplitter(Qt::Horizontal, this);
    pMainLayout->addWidget(m_splitter);

    // 创建左侧日志分类列表
    m_listLogCategory = new QListWidget(m_splitter);
    m_listLogCategory->setSelectionMode(QAbstractItemView::SingleSelection);
    m_splitter->addWidget(m_listLogCategory);

    // 创建右侧日志库列表
    m_listLogLibrary = new QListWidget(m_splitter);
    m_listLogLibrary->setSelectionMode(QAbstractItemView::SingleSelection);
    m_splitter->addWidget(m_listLogLibrary);

    // 设置拉伸比例
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    // 连接信号
    connect(m_listLogCategory, &QListWidget::itemSelectionChanged,
            this, &CWorkLogPage::onCategorySelectionChanged);  // clear() 会触发 itemSelectionChanged，需兼容空选择
    connect(m_listLogLibrary, &QListWidget::itemDoubleClicked,
            this, &CWorkLogPage::onLibraryDoubleClicked);
}

CWorkLogPage::~CWorkLogPage()
{
}

void CWorkLogPage::updateLayout(int leftWidth)
{
    // 调整宽度范围
    int nAdjustedWidth = leftWidth;
    if (nAdjustedWidth < 150)
        nAdjustedWidth = 150;

    m_nLeftWidth = nAdjustedWidth;

    // 更新分割器尺寸
    if (m_splitter)
    {
        QList<int> sizes;
        sizes << nAdjustedWidth << width() - nAdjustedWidth - m_splitter->handleWidth();
        m_splitter->setSizes(sizes);
    }
}

// IObserver 接口实现
void CWorkLogPage::OnDataChanged(const QString& strEventType, void* pData)
{
    Q_UNUSED(pData);
    if (m_pWorkLogManager == nullptr)
        return;

    if (strEventType == QString("ConfigLoaded") || strEventType == QString("CategoryAdded"))
    {
        RefreshCategoryList();
    }
    else if (strEventType == QString("LibraryAdded"))
    {
        RefreshLibraryList();
    }
    else if (strEventType == QString("DataCleared"))
    {
        if (m_listLogCategory)
        {
            m_listLogCategory->clear();
            // 2026-01-25: clear() 会触发 itemSelectionChanged，刷新逻辑需处理空项
        }
        if (m_listLogLibrary)
        {
            m_listLogLibrary->clear();
        }
    }
}

void CWorkLogPage::RefreshCategoryList()
{
    if (m_pWorkLogManager == nullptr || !m_listLogCategory)
        return;

    m_listLogCategory->clear();

    std::vector<QString> categories;
    m_pWorkLogManager->GetAllCategories(categories);

    for (const QString& category : categories)
    {
        qDebug() << "CWorkLogPage::RefreshCategoryList() - Categories: " << category;
        m_listLogCategory->addItem(category);
    }

    if (m_listLogCategory->count() > 0)
    {
        m_listLogCategory->setCurrentRow(0);
        RefreshLibraryList();
    }
    qDebug() << " ";
}

void CWorkLogPage::RefreshLibraryList()
{
    if (m_pWorkLogManager == nullptr || !m_listLogLibrary)
        return;

    QListWidgetItem* pCurrentItem = m_listLogCategory->currentItem();
    if (!pCurrentItem)
        return;

    QString strCategory = pCurrentItem->text();
    if (strCategory.isEmpty())
        return;

    m_listLogLibrary->clear();

    // 获取当前分类下的日志库列表
    std::vector<LogLibraryInfo> libraries;
    if (m_pWorkLogManager->GetLibrariesByCategory(strCategory, libraries))
    {
        // 将日志库名称填充到 UI
        for (const LogLibraryInfo& library : libraries)
        {
            m_listLogLibrary->addItem(library.name);
            qDebug() << "LogPage -  Adding library: " << library.name;
        }
    }
    // 若无数据则保持列表为空
}

void CWorkLogPage::onCategorySelectionChanged()
{
    // clear() 也会触发该信号，RefreshLibraryList 内部已做空值保护
    RefreshLibraryList();
}

void CWorkLogPage::onLibraryDoubleClicked(QListWidgetItem* item)
{
    if (!m_listLogCategory || !m_listLogLibrary || !item)
        return;

    QListWidgetItem* pCategoryItem = m_listLogCategory->currentItem();
    if (!pCategoryItem)
        return;

    QString strLibraryInfo = item->text();
    QString strProjectInfo = pCategoryItem->text();

    CWriteWorkLogDialog dlg(this);
    dlg.SetProjectInfo(strProjectInfo);
    dlg.SetLibraryInfo(strLibraryInfo);
    if (dlg.exec() == QDialog::Accepted)
    {
        QString strLogContent = dlg.GetLogContent();
        QString strLogContenth = dlg.GetLogContenth();

        // 写入普通日志到 logs.txt
        if (!strLogContent.isEmpty())
        {
            m_pWorkLogManager->WriteLog(strLogContent);
        }

        // 写入 H 日志到 logsh.txt
        if (!strLogContenth.isEmpty())
        {
            CWorkLogWriter writer;
            writer.ExecuteH(strLogContenth);
        }
    }
}
