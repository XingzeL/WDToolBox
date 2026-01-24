#pragma execution_character_set("utf-8")

/*
    几个现象：
    1. 如果没有#pragma execution_character_set("utf-8")，里面的中文字会显示乱码
    2. 有时MSVC认不出当前的文件是UTF-8, 需要转换成utf-8 with bom, 否则出现这样的错误: "C2001:常量中有换行符；C2xxx：缺少'(', 缺少';', 缺少')之类的'"
    3. cursor修改utf-8 with bom的文件时会转换成utf-8,但是显示还是with bom。造成Qt一些问题，可以手动重新转成utf-8又转回with bom的形式
    */
#include "src/ui/MainWidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QStyle>

// Global server pointer for cleanup
static QLocalServer* g_singleInstanceServer = nullptr;

// Single instance check using QLocalServer (cross-platform)
bool setupSingleInstance(QApplication& app)
{
    // Create a unique server name based on application name
    QString serverName = "WDToolBox_SingleInstance_Server";

    // Try to connect to existing server
    QLocalSocket socket;
    socket.connectToServer(serverName);

    // If connection succeeds, another instance is running
    if (socket.waitForConnected(500))
    {
        socket.close();
        return false;  // Another instance exists
    }

    // No existing instance, create server for this instance
    QLocalServer::removeServer(serverName);  // Clean up any stale server
    g_singleInstanceServer = new QLocalServer();

    if (!g_singleInstanceServer->listen(serverName))
    {
        // Failed to create server (shouldn't happen if no other instance)
        delete g_singleInstanceServer;
        g_singleInstanceServer = nullptr;
        return false;
    }

    // Server created successfully, this is the first instance
    // Server will be cleaned up when application exits
    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        if (g_singleInstanceServer)
        {
            g_singleInstanceServer->close();
            delete g_singleInstanceServer;
            g_singleInstanceServer = nullptr;
        }
    });

    return true;  // This is the first instance
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application name for single instance check
    app.setApplicationName("WDToolBox(Qt) -- by Xingze");
    app.setOrganizationName("WD");

    // Check if another instance is already running
    if (!setupSingleInstance(app))
    {
        QMessageBox::warning(nullptr, "WDToolBox",
                             "Another instance of WDToolBox is already running.\n"
                             "Only one instance is allowed.");
        return 0;
    }

    // Set encoding to UTF-8 (cross-platform)
    // Qt 5.12 uses QTextCodec
    // Note: On Windows, ConfigReader will try GB2312 as fallback for compatibility with old config files
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    MainWidget widget;
    widget.setWindowTitle("WDToolBox(Qt) - Hoshisawa");

    // Set window icon
    // Method 1: Load from file path (relative to executable)
    QString iconPath = QCoreApplication::applicationDirPath() + "/myico.ico";
    if (QFileInfo::exists(iconPath))
    {
        widget.setWindowIcon(QIcon(iconPath));
    }
    else
    {
        // Method 2: Try resource file (if .qrc exists)
        // widget.setWindowIcon(QIcon(":/icons/WDToolBox.ico"));

        // Method 3: Use system standard application icon as fallback
        QStyle* pStyle = app.style();
        if (pStyle)
        {
            // Use standard file icon as fallback
            widget.setWindowIcon(pStyle->standardIcon(QStyle::SP_FileIcon));
        }
    }

    widget.resize(1000, 700);
    widget.show();

    return app.exec();
}
