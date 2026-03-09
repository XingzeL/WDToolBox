// ToolManagerPage.cpp: Tool Manager Page implementation
//
#pragma execution_character_set("utf-8")
#include "ToolManagerPage.h"
#include "../core/ToolManager.h"
#include "../core/ToolInfo.h"
#include <QListWidgetItem>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QContextMenuEvent>
#include <QMenu>
#include <QApplication>
#include <QInputDialog>
#include <QClipboard>
#include <QDesktopServices>
#include <QBrush>
#include <QFont>

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
    // ??????????????????
    m_listTool->setAcceptDrops(true);
    m_listTool->setDragDropMode(QAbstractItemView::DropOnly);
    // ??????
    m_listTool->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listTool, &QListWidget::customContextMenuRequested,
            this, &CToolManagerPage::onToolContextMenu);
    m_splitter->addWidget(m_listTool);

    // Set stretch factors
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    // Connect signals
    connect(m_listCategory, &QListWidget::itemSelectionChanged,
            this, &CToolManagerPage::onCategorySelectionChanged);
    connect(m_listTool, &QListWidget::itemDoubleClicked,
            this, &CToolManagerPage::onToolDoubleClicked);

    setAcceptDrops(true);
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

    QString strCurrentCategory;
    QListWidgetItem* pCurrentItem = m_listCategory->currentItem();
    if (pCurrentItem)
    {
        strCurrentCategory = pCurrentItem->text();
    }

    m_listCategory->clear();

    std::vector<QString> categories;
    m_pToolManager->GetAllCategories(categories);

    for (const QString& category : categories)
    {
        m_listCategory->addItem(category);
    }


    if (!strCurrentCategory.isEmpty())
    {

        QList<QListWidgetItem*> items = m_listCategory->findItems(strCurrentCategory, Qt::MatchExactly);
        if (!items.isEmpty())
        {
            m_listCategory->setCurrentItem(items.first());
            RefreshToolList();
            return;
        }
    }

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
            applyHighlightVisual(pItem, tool);
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

void CToolManagerPage::onToolContextMenu(const QPoint& pos)
{
    if (!m_listTool || !m_pToolManager)
        return;

    QListWidgetItem* pItem = m_listTool->itemAt(pos);
    if (!pItem)
        return;

    m_listTool->setCurrentItem(pItem);

    QMenu menu(this);
    ToolInfo tool = pItem->data(Qt::UserRole).value<ToolInfo>();
    QAction* pRenameAction = menu.addAction("重命名");
    QAction* pRemoveAction = menu.addAction("移除");
    QAction* pGetFullPathAction = menu.addAction("获取路径");
    QAction* pOpenInExplorerAction = menu.addAction("在资源管理器中打开");
    QAction* pToggleHighlightAction = menu.addAction(tool.highlighted ? "取消高亮" : "高亮");

    QAction* pSelectedAction = menu.exec(m_listTool->mapToGlobal(pos));

    if (!pSelectedAction)
        return;

    if (pSelectedAction == pRemoveAction)
    {
        onRemoveTool();
    }
    else if (pSelectedAction == pRenameAction)
    {
        onRenameTool();
    }
    else if (pSelectedAction == pGetFullPathAction)
    {
        onGetFullPath(pItem);
    }
    else if (pSelectedAction == pOpenInExplorerAction)
    {
        onOpenInExplorer(pItem);
    }
    else if (pSelectedAction == pToggleHighlightAction)
    {
        onToggleHighlight(pItem);
    }
}

void CToolManagerPage::onRemoveTool()
{
    if (!m_pToolManager || !m_listTool || !m_listCategory)
        return;

    QListWidgetItem* pToolItem = m_listTool->currentItem();
    if (!pToolItem)
        return;

    QListWidgetItem* pCategoryItem = m_listCategory->currentItem();
    if (!pCategoryItem)
        return;

    QString strCategory = pCategoryItem->text();
    ToolInfo tool = pToolItem->data(Qt::UserRole).value<ToolInfo>();

    int nRet = QMessageBox::question(this, "确认删除",
                                     QString("确定要删除工具 \"%1\" 吗？").arg(tool.name),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if (nRet != QMessageBox::Yes)
        return;

    if (m_pToolManager->RemoveTool(strCategory, tool.name))
    {
        if (m_pToolManager->SaveToConfig())
        {
            RefreshToolList();
        }
        else
        {
            QMessageBox::warning(this, "警告", "工具已删除，但保存配置文件失败！");
        }
    }
    else
    {
        QMessageBox::warning(this, "错误", "删除工具失败!");
    }
}

void CToolManagerPage::onGetFullPath(QListWidgetItem* pItem)
{
    // TODO: 实现获取工具的完整路径
    if (!m_pToolManager || !pItem)
        return;
    ToolInfo tool = pItem->data(Qt::UserRole).value<ToolInfo>();
    QString strFullPath = tool.path;
    // 将路径内容复制到剪贴板
    QApplication::clipboard()->setText(strFullPath);
    QMessageBox::information(this, "提示", QString("工具路径已复制到剪贴板：%1").arg(strFullPath));
}

void CToolManagerPage::onOpenInExplorer(QListWidgetItem* pItem)
{
    if (!pItem)
        return;

    ToolInfo tool = pItem->data(Qt::UserRole).value<ToolInfo>();
    QString strPath = tool.path.trimmed();
    if (strPath.isEmpty())
    {
        QMessageBox::warning(this, "警告", "工具路径为空，无法打开所在文件夹。");
        return;
    }

    QFileInfo fileInfo(strPath);
    QString strFolderPath = fileInfo.isDir() ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
    if (strFolderPath.isEmpty() || !QFileInfo::exists(strFolderPath))
    {
        QMessageBox::warning(this, "警告", QString("所在文件夹不存在：%1").arg(strFolderPath));
        return;
    }

    const bool bOpened = QDesktopServices::openUrl(QUrl::fromLocalFile(strFolderPath));
    if (!bOpened)
    {
        QMessageBox::warning(this, "错误", QString("无法打开文件夹：%1").arg(strFolderPath));
    }
}

void CToolManagerPage::onToggleHighlight(QListWidgetItem* pItem)
{
    if (!m_pToolManager || !m_listCategory || !pItem)
        return;

    QListWidgetItem* pCategoryItem = m_listCategory->currentItem();
    if (!pCategoryItem)
        return;

    QString strCategory = pCategoryItem->text();
    ToolInfo tool = pItem->data(Qt::UserRole).value<ToolInfo>();
    const bool bNewHighlighted = !tool.highlighted;

    if (!m_pToolManager->SetToolHighlighted(strCategory, tool.name, bNewHighlighted))
    {
        QMessageBox::warning(this, "错误", "更新高亮状态失败。");
        return;
    }

    if (!m_pToolManager->SaveToConfig())
    {
        QMessageBox::warning(this, "警告", "高亮状态已更新，但保存配置文件失败！");
    }

    tool.highlighted = bNewHighlighted;
    pItem->setData(Qt::UserRole, QVariant::fromValue(tool));
    applyHighlightVisual(pItem, tool);
}

void CToolManagerPage::applyHighlightVisual(QListWidgetItem* pItem, const ToolInfo& tool)
{
    if (!pItem)
        return;

    QFont font = pItem->font();
    if (tool.highlighted)
    {
        font.setBold(true);
        pItem->setFont(font);
        pItem->setBackground(QBrush(QColor(255, 245, 170)));
        pItem->setForeground(QBrush(QColor(80, 60, 0)));
    }
    else
    {
        font.setBold(false);
        pItem->setFont(font);
        pItem->setData(Qt::BackgroundRole, QVariant());
        pItem->setData(Qt::ForegroundRole, QVariant());
    }
}

void CToolManagerPage::onRenameTool()
{
    if (!m_pToolManager || !m_listTool || !m_listCategory)
        return;

    QListWidgetItem* pToolItem = m_listTool->currentItem();
    if (!pToolItem)
        return;

    QListWidgetItem* pCategoryItem = m_listCategory->currentItem();
    if (!pCategoryItem)
        return;

    QString strCategory = pCategoryItem->text();
    ToolInfo tool = pToolItem->data(Qt::UserRole).value<ToolInfo>();

    bool bOk = false;
    QString strNewName = QInputDialog::getText(this, "重命名工具",
                                               "请输入新名称：",
                                               QLineEdit::Normal,
                                               tool.name,
                                               &bOk);

    if (!bOk || strNewName.isEmpty() || strNewName == tool.name)
        return;

    if (m_pToolManager->RenameTool(strCategory, tool.name, strNewName))
    {
        if (m_pToolManager->SaveToConfig())
        {
            RefreshToolList();
        }
        else
        {
            QMessageBox::warning(this, "警告", "工具已重命名，但保存配置文件失败！");
        }
    }
    else
    {
        QMessageBox::warning(this, "错误", QString("重命名失败！可能是名称 \"%1\" 已存在。").arg(strNewName));
    }
}

void CToolManagerPage::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void CToolManagerPage::dropEvent(QDropEvent* event)
{
    if (!m_pToolManager || !event->mimeData()->hasUrls())
    {
        event->ignore();
        return;
    }

    QListWidgetItem* pCurrentCategoryItem = m_listCategory->currentItem();
    if (!pCurrentCategoryItem)
    {
        QMessageBox::warning(this, "警告", "请先选择一个分类！");
        event->ignore();
        return;
    }

    QString strCategory = pCurrentCategoryItem->text();
    if (strCategory.isEmpty())
    {
        event->ignore();
        return;
    }


    QList<QUrl> urls = event->mimeData()->urls();
    int nAddedCount = 0;
    int nFailedCount = 0;

    for (const QUrl& url : urls)
    {
        if (!url.isLocalFile())
            continue;

        QString strFilePath = url.toLocalFile();
        QFileInfo fileInfo(strFilePath);


        if (!fileInfo.exists())
        {
            nFailedCount++;
            continue;
        }

        QString strToolName = fileInfo.fileName();
        if (strToolName.isEmpty())
        {
            strToolName = QDir(strFilePath).dirName();
        }

        QString strOriginalToolName = strToolName;
        int nIndex = 1;
        while (true)
        {
            std::vector<ToolInfo> existingTools;
            if (m_pToolManager->GetToolsByCategory(strCategory, existingTools))
            {
                bool bExists = false;
                for (const ToolInfo& tool : existingTools)
                {
                    if (tool.name == strToolName)
                    {
                        bExists = true;
                        break;
                    }
                }
                if (!bExists)
                    break;
            }
            else
            {
                break;
            }

            strToolName = strOriginalToolName + QString(" (%1)").arg(nIndex);
            nIndex++;
        }

        QString strFullPath = QDir::toNativeSeparators(strFilePath);

        m_pToolManager->AddTool(strCategory, strToolName, strFullPath, true);
        nAddedCount++;
    }

    if (nAddedCount > 0)
    {
        if (!m_pToolManager->SaveToConfig())
        {
            QMessageBox::warning(this, "警告", "工具已添加，但保存配置文件失败！");
        }
    }

    if (nAddedCount > 0 && nFailedCount == 0)
    {

    }
    else if (nAddedCount > 0 && nFailedCount > 0)
    {
        QMessageBox::warning(this, "警告", QString("成功添加 %1 个工具，%2 个失败").arg(nAddedCount).arg(nFailedCount));
    }
    else if (nAddedCount == 0)
    {
        QMessageBox::warning(this, "警告", "未能添加任何工具，请检查文件路径是否正确。");
    }

    event->acceptProposedAction();
}
