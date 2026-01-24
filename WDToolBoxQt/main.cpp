#include "src/ui/MainWidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置编码为UTF-8（跨平台）
    // Qt 5.12 使用 QTextCodec
    // 注意：在Windows上，ConfigReader会尝试GB2312作为fallback以兼容旧配置文件
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    MainWidget widget;
    widget.setWindowTitle("WDToolBox");
    // widget.setWindowIcon(QIcon(":/icons/WDToolBox.ico"));  // 如果有资源文件
    widget.resize(1000, 700);
    widget.show();

    return app.exec();
}
