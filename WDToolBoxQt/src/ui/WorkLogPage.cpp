// WorkLogPage.cpp: ????????
//
#include "WorkLogPage.h"
#include "../core/WorkLogManager.h"
#include "WriteWorkLogDialog.h"
#include "../infrastructure/WorkLogWriter.h"
#include <QListWidgetItem>
#include <QDialog>

CWorkLogPage::CWorkLogPage(QWidget* parent)
    : TabPageBase(parent)
    , m_listLogCategory(nullptr)
    , m_listLogLibrary(nullptr)
    , m_splitter(nullptr)
    , m_pWorkLogManager(nullptr)
    , m_nLeftWidth(200)
{
    // ????
    QHBoxLayout* pMainLayout = new QHBoxLayout(this);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);

    // ?????
    m_splitter = new QSplitter(Qt::Horizontal, this);
    pMainLayout->addWidget(m_splitter);

    // ??????????
    m_listLogCategory = new QListWidget(m_splitter);
    m_listLogCategory->setSelectionMode(QAbstractItemView::SingleSelection);
    m_splitter->addWidget(m_listLogCategory);

    // ?????????
    m_listLogLibrary = new QListWidget(m_splitter);
    m_listLogLibrary->setSelectionMode(QAbstractItemView::SingleSelection);
    m_splitter->addWidget(m_listLogLibrary);

    // ???????
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    // ?????
    connect(m_listLogCategory, &QListWidget::itemSelectionChanged,
            this, &CWorkLogPage::onCategorySelectionChanged);
    connect(m_listLogLibrary, &QListWidget::itemDoubleClicked,
            this, &CWorkLogPage::onLibraryDoubleClicked);
}

CWorkLogPage::~CWorkLogPage()
{
}

void CWorkLogPage::updateLayout(int leftWidth)
{
    // ??????????
    int nAdjustedWidth = leftWidth;
    if (nAdjustedWidth < 150)
        nAdjustedWidth = 150;

    m_nLeftWidth = nAdjustedWidth;

    // ???????
    if (m_splitter)
    {
        QList<int> sizes;
        sizes << nAdjustedWidth << width() - nAdjustedWidth - m_splitter->handleWidth();
        m_splitter->setSizes(sizes);
    }
}

// IObserver ????
void CWorkLogPage::OnDataChanged(const QString& strEventType, void* pData)
{
    Q_UNUSED(pData);
    if (m_pWorkLogManager == nullptr)
        return;

    // ????????UI
    if (strEventType == QString("ConfigLoaded") || strEventType == QString("CategoryAdded"))
    {
        // ????????????????
        RefreshCategoryList();
    }
    else if (strEventType == QString("LibraryAdded"))
    {
        // ?????????
        RefreshLibraryList();
    }
    else if (strEventType == QString("DataCleared"))
    {
        // ?????????
        if (m_listLogCategory)
        {
            m_listLogCategory->clear();
        }
        if (m_listLogLibrary)
        {
            m_listLogLibrary->clear();
        }
    }
}

// ??????
void CWorkLogPage::RefreshCategoryList()
{
    if (m_pWorkLogManager == nullptr || !m_listLogCategory)
        return;

    m_listLogCategory->clear();

    std::vector<QString> categories;
    m_pWorkLogManager->GetAllCategories(categories);

    for (const QString& category : categories)
    {
        m_listLogCategory->addItem(category);
    }

    // ???????
    if (m_listLogCategory->count() > 0)
    {
        m_listLogCategory->setCurrentRow(0);
        RefreshLibraryList();
    }
}

// ???????
void CWorkLogPage::RefreshLibraryList()
{
    if (m_pWorkLogManager == nullptr || !m_listLogLibrary)
        return;

    // ?????????
    QListWidgetItem* pCurrentItem = m_listLogCategory->currentItem();
    if (!pCurrentItem)
        return;

    QString strCategory = pCurrentItem->text();
    if (strCategory.isEmpty())
        return;

    m_listLogLibrary->clear();

    std::vector<LogLibraryInfo>& libraries = m_pWorkLogManager->GetLibrariesByCategory(strCategory);
    for (const LogLibraryInfo& library : libraries)
    {
        m_listLogLibrary->addItem(library.name);
    }
}

void CWorkLogPage::onCategorySelectionChanged()
{
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

        // ?????????? logs.txt
        if (!strLogContent.isEmpty())
        {
            m_pWorkLogManager->WriteLog(strLogContent);
        }

        // ????????? logsh.txt
        if (!strLogContenth.isEmpty())
        {
            CWorkLogWriter writer;
            writer.ExecuteH(strLogContenth);
        }
    }
}
