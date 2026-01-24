#include "src/ui/MainWidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QDir>

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
    app.setApplicationName("WDToolBox");
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
    widget.setWindowTitle("WDToolBox");
    // widget.setWindowIcon(QIcon(":/icons/WDToolBox.ico"));  // If resource file exists
    widget.resize(1000, 700);
    widget.show();

    return app.exec();
}
