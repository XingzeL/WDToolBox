// MainWidget.h: 主窗口
//
#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include "../core/ToolManager.h"
#include "../core/WorkLogManager.h"
#include "../infrastructure/ConfigReader.h"
#include "../infrastructure/ConfigFileWatcher.h"
#include "ToolManagerPage.h"
#include "WorkLogPage.h"

// 主窗口类
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget* parent = nullptr);
    virtual ~MainWidget();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private slots:
    void onTabChanged(int index);
    void onToolConfigFileChanged(const QString& path);
    void onLogConfigFileChanged(const QString& path);

private:
    void InitializeControls();
    void LoadToolCategories();
    void LoadLogCategories();
    void StartConfigFileWatcher();

    // 分页控件
    QTabWidget* m_tabCtrl;          // 标签页控件

    // 分页对象（每个分页封装自己的控件和逻辑）
    CToolManagerPage* m_toolManagerPage;  // 工具管理器分页
    CWorkLogPage* m_workLogPage;           // 工作日志分页

    // 配置读取器（统一管理）
    CIniConfigReader* m_toolConfigReader;    // 工具配置读取器
    CIniConfigReader* m_logConfigReader;     // 日志配置读取器

    // 管理器（使用依赖注入，内部管理 Executor）
    CToolManager* m_toolManager;
    CWorkLogManager* m_workLogger;

    // 配置文件监控器
    CConfigFileWatcher* m_configWatcher;

    // 布局参数
    int m_nCategoryListWidth;    // 左侧分类列表宽度（可调整）
};
