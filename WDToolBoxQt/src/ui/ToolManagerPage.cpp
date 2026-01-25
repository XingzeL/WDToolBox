// ToolManagerPage.cpp: Tool Manager Page implementation
//
#include "ToolManagerPage.h"
#include "../core/ToolManager.h"
#include "../core/ToolInfo.h"
#include <QListWidgetItem>

CToolManagerPage::CToolManagerPage(QWidget* parent)
    : TabPageBase(parent)
    , m_listCategory(nullptr)
    , m_listTool(nullptr)
    , m_splitter(nullptr)
    , m_pToolManager(nullptr)
    , m_nLeftWidth(200)
{
    // Create main layout
    QHBoxLayout* pMainLayout = new QHBoxLayout(this);
    pMainLayout->setContentsMargins(0, 0, 0, 0);
    pMainLayout->setSpacing(0);

    // Create splitter
    m_splitter = new QSplitter(Qt::Horizontal, this);
    pMainLayout->addWidget(m_splitter);

    // Create category list
    m_listCategory = new QListWidget(m_splitter);
    m_listCategory->setSelectionMode(QAbstractItemView::SingleSelection);
    m_splitter->addWidget(m_listCategory);

    // Create tool list
    m_listTool = new QListWidget(m_splitter);
    m_listTool->setViewMode(QListWidget::IconMode);
    m_listTool->setResizeMode(QListWidget::Adjust);
    m_listTool->setSpacing(10);
    m_splitter->addWidget(m_listTool);

    // Set stretch factors
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    // Connect signals
    connect(m_listCategory, &QListWidget::itemSelectionChanged,
            this, &CToolManagerPage::onCategorySelectionChanged);
    connect(m_listTool, &QListWidget::itemDoubleClicked,
            this, &CToolManagerPage::onToolDoubleClicked);
}

CToolManagerPage::~CToolManagerPage()
{
}

void CToolManagerPage::updateLayout(int leftWidth)
{
    // Adjust width
    int nAdjustedWidth = leftWidth;
    if (nAdjustedWidth < 150)
        nAdjustedWidth = 150;

    m_nLeftWidth = nAdjustedWidth;

    // Update splitter sizes
    if (m_splitter)
    {
        QList<int> sizes;
        sizes << nAdjustedWidth << width() - nAdjustedWidth - m_splitter->handleWidth();
        m_splitter->setSizes(sizes);
    }
}

// IObserver interface implementation
void CToolManagerPage::OnDataChanged(const QString& strEventType, void* pData)
{
    Q_UNUSED(pData);
    if (m_pToolManager == nullptr)
        return;

    // Update UI
    if (strEventType == QString("ConfigLoaded") || strEventType == QString("CategoryAdded"))
    {
        // Refresh category list
        RefreshCategoryList();
    }
    else if (strEventType == QString("ToolAdded"))
    {
        // Refresh tool list
        RefreshToolList();
    }
    else if (strEventType == QString("DataCleared"))
    {
        // Clear lists
        if (m_listCategory)
        {
            m_listCategory->clear();
        }
        if (m_listTool)
        {
            m_listTool->clear();
        }
    }
}

// Refresh category list
void CToolManagerPage::RefreshCategoryList()
{
    if (m_pToolManager == nullptr || !m_listCategory)
        return;

    m_listCategory->clear();

    std::vector<QString> categories;
    m_pToolManager->GetAllCategories(categories);

    for (const QString& category : categories)
    {
        m_listCategory->addItem(category);
    }

    // Select first category
    if (m_listCategory->count() > 0)
    {
        m_listCategory->setCurrentRow(0);
        RefreshToolList();
    }
}

// Refresh tool list
void CToolManagerPage::RefreshToolList()
{
    if (m_pToolManager == nullptr || !m_listTool)
        return;

    // Get selected category
    QListWidgetItem* pCurrentItem = m_listCategory->currentItem();
    if (!pCurrentItem)
        return;

    QString strCategory = pCurrentItem->text();
    if (strCategory.isEmpty())
        return;

    m_listTool->clear();

    std::vector<ToolInfo> tools;
    if (m_pToolManager->GetToolsByCategory(strCategory, tools))
    {
        for (const ToolInfo& tool : tools)
        {
            QListWidgetItem* pItem = new QListWidgetItem(tool.icon, tool.name, m_listTool);
            pItem->setData(Qt::UserRole, QVariant::fromValue(tool));
        }
    }
}

void CToolManagerPage::onCategorySelectionChanged()
{
    RefreshToolList();
}

void CToolManagerPage::onToolDoubleClicked(QListWidgetItem* item)
{
    if (!m_pToolManager || !item)
        return;

    ToolInfo tool = item->data(Qt::UserRole).value<ToolInfo>();
    m_pToolManager->ExecuteTool(tool);
}
