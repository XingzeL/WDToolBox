// MainWidget.cpp: 主窗口实现
//
/*
    两个重要编码相关：
    1. 4.12版本Creator似乎无法直接识别utf-8, 需要将文件转换成utf-8 with bom, 否则出现这样的错误: "C2001:常量中有换行符；C2xxx：缺少'(', 缺少';', 缺少')之类的'"
    2. 如果没有#pragma execution_character_set("utf-8")， 里面的中文字会显示乱码

    几个现象：
    1. cursor中
*/
#pragma execution_character_set("utf-8") //如果不写这个，里面的中文字会出现乱码
#include "MainWidget.h"
#include <QMessageBox>
#include <QResizeEvent>
#include <QFileInfo>
#include <QWidget>
#include <QVBoxLayout>

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent)
    , m_tabCtrl(nullptr)
    , m_toolManagerPage(nullptr)
    , m_workLogPage(nullptr)
    , m_toolConfigReader(nullptr)
    , m_logConfigReader(nullptr)
    , m_toolManager(nullptr)
    , m_workLogger(nullptr)
    , m_configWatcher(nullptr)
    , m_nCategoryListWidth(200)
{
    // 创建配置读取器
    m_toolConfigReader = new CIniConfigReader();
    m_logConfigReader = new CIniConfigReader();

    // 创建管理器（注入配置读取器）
    m_toolManager = new CToolManager(m_toolConfigReader, nullptr);
    m_workLogger = new CWorkLogManager(m_logConfigReader, nullptr);

    // 创建配置文件监控器
    m_configWatcher = new CConfigFileWatcher(this);

    // 初始化控件
    InitializeControls();

    // 加载工具分类（观察者会自动更新UI）
    LoadToolCategories();

    // 启动配置文件监控
    StartConfigFileWatcher();
}

MainWidget::~MainWidget()
{
    // 清理资源
    if (m_toolManager)
    {
        delete m_toolManager;
        m_toolManager = nullptr;
    }

    if (m_workLogger)
    {
        delete m_workLogger;
        m_workLogger = nullptr;
    }

    if (m_toolConfigReader)
    {
        delete m_toolConfigReader;
        m_toolConfigReader = nullptr;
    }

    if (m_logConfigReader)
    {
        delete m_logConfigReader;
        m_logConfigReader = nullptr;
    }
}

void MainWidget::InitializeControls()
{
    // 创建主布局
    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建标签页控件
    m_tabCtrl = new QTabWidget(this);
    pMainLayout->addWidget(m_tabCtrl);

    // 创建分页对象
    m_toolManagerPage = new CToolManagerPage();
    m_workLogPage = new CWorkLogPage();

    // 添加分页到标签页
    m_tabCtrl->addTab(m_toolManagerPage, "工具管理器");
    m_tabCtrl->addTab(m_workLogPage, "工作日志");

    // 设置 Manager 指针（用于观察者模式）
    m_toolManagerPage->SetToolManager(m_toolManager);
    m_workLogPage->SetWorkLogManager(m_workLogger);

    // 注册观察者
    // 这个接口是继承自CObservable的，将工具管理器加到m_vecObservers中
    m_toolManager->AddObserver(m_toolManagerPage);
    m_workLogger->AddObserver(m_workLogPage);

    // 连接标签页切换信号
    connect(m_tabCtrl, &QTabWidget::currentChanged, this, &MainWidget::onTabChanged);
}

void MainWidget::LoadToolCategories()
{
    // 从配置文件加载工具，如果失败则使用默认配置
    if (!m_toolManager->LoadFromConfig(""))
    {
        // 配置文件不存在或加载失败，将使用默认配置
        QMessageBox::information(this, "提示", "配置文件不存在或加载失败，将使用默认配置。");
        m_toolManager->LoadDefaultTools();
    }

    // 加载工具图标
    QListWidget* pToolList = m_toolManagerPage->GetToolList();
    if (pToolList)
    {
        m_toolManager->LoadToolIcons(pToolList);
    }

    // 注意：分类列表和工具列表的填充现在由观察者模式自动处理
    // LoadFromConfig 会触发 ConfigLoaded 事件，观察者会自动更新UI
    // 这里保留手动刷新作为后备
    m_toolManagerPage->RefreshCategoryList();
}

void MainWidget::LoadLogCategories()
{
    // 从配置文件加载日志库，如果失败则使用默认配置
    if (!m_workLogger->LoadFromConfig(""))
    {
        // 配置文件不存在或加载失败，将使用默认配置
        QMessageBox::information(this, "提示", "日志配置文件不存在或加载失败，将使用默认配置。");
        m_workLogger->LoadDefaultLibraries();
    }

    // 注意：分类列表和库列表的填充现在由观察者模式自动处理
    // LoadFromConfig 会触发 ConfigLoaded 事件，观察者会自动更新UI
    // 这里保留手动刷新作为后备
    m_workLogPage->RefreshCategoryList();
}

void MainWidget::StartConfigFileWatcher()
{
    // 获取工具配置文件路径
    QString strToolConfigPath = m_toolManager->GetDefaultConfigPath();
    if (QFileInfo::exists(strToolConfigPath))
    {
        // 启动工具配置文件监控
        m_configWatcher->StartWatch(strToolConfigPath,
            [this](const QString& strFilePath) {
                // 文件变化时重新加载配置
                m_toolManager->LoadFromConfig(strFilePath);
                // 观察者会自动更新UI
            });
    }

    // 获取日志配置文件路径
    QString strLogConfigPath = m_workLogger->GetDefaultConfigPath();
    if (QFileInfo::exists(strLogConfigPath))
    {
        // 启动日志配置文件监控
        m_configWatcher->StartWatch(strLogConfigPath,
            [this](const QString& strFilePath) {
                // 文件变化时重新加载配置
                m_workLogger->LoadFromConfig(strFilePath);
                // 观察者会自动更新UI
            });
    }
}

void MainWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (m_tabCtrl)
    {
        // 更新所有分页的布局
        if (m_toolManagerPage)
        {
            m_toolManagerPage->updateLayout(m_nCategoryListWidth);
        }
        if (m_workLogPage)
        {
            m_workLogPage->updateLayout(m_nCategoryListWidth);
        }
    }
}

void MainWidget::onTabChanged(int index)
{
    // 切换到工作日志标签页时，加载日志分类
    if (index == 1)
    {
        LoadLogCategories();
    }
}

void MainWidget::onToolConfigFileChanged(const QString& path)
{
    // 文件变化时重新加载配置
    m_toolManager->LoadFromConfig(path);
}

void MainWidget::onLogConfigFileChanged(const QString& path)
{
    // 文件变化时重新加载配置
    m_workLogger->LoadFromConfig(path);
}
