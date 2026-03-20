#include "PromptComposerPage.h"
#include "EditPromptFieldsDialog.h"
#include "../infrastructure/ConfigReader.h"
#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVector>

namespace {

PromptRolePreset makePreset(
    const QString& displayName,
    const QString& role,
    const QString& context,
    const QString& constraints,
    const QString& outputFormat,
    const QString& acceptanceCriteria)
{
    PromptRolePreset preset;
    preset.displayName = displayName;
    preset.role = role;
    preset.context = context;
    preset.task.clear();
    preset.constraints = constraints;
    preset.outputFormat = outputFormat;
    preset.acceptanceCriteria = acceptanceCriteria;
    return preset;
}

static QVector<PromptRolePreset> defaultPresets()
{
    QVector<PromptRolePreset> v;

    v.append(makePreset(
        QString::fromUtf8(u8"\u81ea\u5b9a\u4e49"),
        QString(),
        QString(),
        QString(),
        QString(),
        QString()));

    v.append(makePreset(
        QString::fromUtf8(u8"MFC \u4e13\u5bb6"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u8d44\u6df1 Windows \u684c\u9762\u5f00\u53d1\u5de5\u7a0b\u5e08\uff0c\u64c5\u957f MFC\u3001Win32 API\u3001\u8d44\u6e90\u7f16\u8f91\u5668\u3001\u6d88\u606f\u6620\u5c04\u4ee5\u53ca\u8001\u9879\u76ee\u7ef4\u62a4\u3002"),
        QString::fromUtf8(u8"\u5f53\u524d\u9879\u76ee\u662f MFC \u6216 Qt/MFC \u6df7\u5408\u684c\u9762\u5e94\u7528\uff0c\u8bf7\u4f18\u5148\u91c7\u7528\u4f4e\u98ce\u9669\u3001\u517c\u5bb9\u73b0\u6709\u4ee3\u7801\u98ce\u683c\u548c\u5de5\u7a0b\u7ed3\u6784\u7684\u65b9\u6848\u3002"),
        QString::fromUtf8(
            u8"- \u4f7f\u7528\u517c\u5bb9 MSVC \u7684 C++ \u5199\u6cd5\u3002\n"
            u8"- \u4e0d\u8981\u7834\u574f\u73b0\u6709\u6d88\u606f\u6620\u5c04\u3001DDX/DDV\u3001\u8d44\u6e90 ID \u548c\u5bf9\u8bdd\u6846\u6d41\u7a0b\u3002\n"
            u8"- \u5982\u679c\u5b58\u5728\u4e0d\u786e\u5b9a\u524d\u63d0\uff0c\u8981\u660e\u786e\u5199\u51fa\u5047\u8bbe\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u95ee\u9898\u5206\u6790\u3001\u4fee\u6539\u65b9\u6848\u3001\u5173\u952e\u4ee3\u7801\u3001\u98ce\u9669\u8bf4\u660e\u3001\u9a8c\u8bc1\u65b9\u6cd5\u3002"),
        QString::fromUtf8(u8"\u65b9\u6848\u5e94\u53ef\u76f4\u63a5\u843d\u5730\uff0c\u7b26\u5408 MFC \u5de5\u7a0b\u4e60\u60ef\uff0c\u5e76\u8bf4\u660e\u662f\u5426\u9700\u8981\u8c03\u6574\u8d44\u6e90\u6587\u4ef6\u3001\u83dc\u5355\u3001\u5bf9\u8bdd\u6846\u6216\u5b57\u7b26\u4e32\u8868\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"Qt \u4e13\u5bb6"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u7cbe\u901a Qt Widgets\u3001\u4fe1\u53f7\u69fd\u3001\u6a21\u578b\u89c6\u56fe\u3001Windows \u90e8\u7f72\u4e0e\u8c03\u8bd5\u7684\u9ad8\u7ea7 Qt \u5de5\u7a0b\u5e08\u3002"),
        QString::fromUtf8(u8"\u9879\u76ee\u57fa\u4e8e Qt 5 Widgets\uff0c\u4f18\u5148\u4f7f\u7528 Qt \u539f\u751f API\uff0c\u5c3d\u91cf\u907f\u514d\u65e0\u5fc5\u8981\u7684\u5e73\u53f0\u4e13\u5c5e\u5b9e\u73b0\u3002"),
        QString::fromUtf8(
            u8"- \u4f18\u5148\u517c\u5bb9 Qt 5.12\u3002\n"
            u8"- \u9075\u5faa QObject \u7236\u5b50\u5bf9\u8c61\u7ba1\u7406\u548c RAII\u3002\n"
            u8"- \u6d89\u53ca\u7ebf\u7a0b\u65f6\u660e\u786e\u8bf4\u660e\u7ebf\u7a0b\u8fb9\u754c\u548c UI \u7ebf\u7a0b\u9650\u5236\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u5b9e\u73b0\u601d\u8def\u3001\u6838\u5fc3\u4ee3\u7801\u3001\u6784\u5efa\u8bf4\u660e\u3001\u8fb9\u754c\u60c5\u51b5\u3001\u6d4b\u8bd5\u5efa\u8bae\u3002"),
        QString::fromUtf8(u8"\u4ee3\u7801\u5e94\u4fbf\u4e8e\u76f4\u63a5\u590d\u5236\u8fdb\u73b0\u6709\u5de5\u7a0b\uff0c\u5e76\u6307\u51fa\u53ef\u80fd\u5f71\u54cd\u7684 UI\u3001\u4fe1\u53f7\u69fd\u8fde\u63a5\u548c\u5de5\u7a0b\u6587\u4ef6\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"\u4ee3\u7801\u5ba1\u67e5"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u4e25\u683c\u4e14\u52a1\u5b9e\u7684\u4ee3\u7801\u5ba1\u67e5\u8005\uff0c\u91cd\u70b9\u5173\u6ce8\u6b63\u786e\u6027\u3001\u7a33\u5b9a\u6027\u3001\u5b89\u5168\u6027\u3001\u53ef\u7ef4\u62a4\u6027\u3002"),
        QString::fromUtf8(u8"\u8bf7\u7ad9\u5728\u5c0f\u578b C++/Qt \u56e2\u961f\u534f\u4f5c\u573a\u666f\u4e0b\uff0c\u5ba1\u67e5\u7ed9\u5b9a\u6539\u52a8\u6216\u6587\u4ef6\u3002"),
        QString::fromUtf8(
            u8"- \u5148\u8bf4\u4e25\u91cd\u95ee\u9898\uff0c\u518d\u8bf4\u4e00\u822c\u5efa\u8bae\u3002\n"
            u8"- \u6bcf\u6761\u610f\u89c1\u90fd\u8981\u7ed9\u51fa\u539f\u56e0\u3002\n"
            u8"- \u4fdd\u6301\u7b80\u6d01\u76f4\u63a5\uff0c\u4e0d\u8981\u6cdb\u6cdb\u800c\u8c08\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u95ee\u9898\u6e05\u5355\uff08\u6807\u6ce8\u4e25\u91cd\u7ea7\u522b\uff09\u3001\u5f85\u786e\u8ba4\u95ee\u9898\u3001\u5efa\u8bae\u4fee\u6539\u65b9\u5411\u3002"),
        QString::fromUtf8(u8"\u6bcf\u4e2a\u95ee\u9898\u90fd\u5e94\u5b9a\u4f4d\u5230\u5177\u4f53\u6587\u4ef6\u3001\u51fd\u6570\u6216\u903b\u8f91\u70b9\uff0c\u5e76\u8bf4\u660e\u53ef\u80fd\u9020\u6210\u7684\u540e\u679c\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"Bug \u6392\u67e5"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u64c5\u957f\u5b9a\u4f4d\u590d\u6742\u7f3a\u9677\u7684\u9ad8\u7ea7\u8c03\u8bd5\u5de5\u7a0b\u5e08\uff0c\u80fd\u591f\u5feb\u901f\u7f29\u5c0f\u8303\u56f4\u5e76\u63d0\u51fa\u9a8c\u8bc1\u65b9\u6848\u3002"),
        QString::fromUtf8(u8"\u7528\u6237\u4f1a\u63d0\u4f9b\u73b0\u8c61\u3001\u65e5\u5fd7\u3001\u5d29\u6e83\u6808\u6216\u590d\u73b0\u6b65\u9aa4\uff0c\u4f60\u9700\u8981\u4ece\u684c\u9762\u5e94\u7528\u5b9e\u9645\u6392\u969c\u89d2\u5ea6\u7ed9\u51fa\u5206\u6790\u3002"),
        QString::fromUtf8(
            u8"- \u4f18\u5148\u5217\u51fa\u6700\u53ef\u80fd\u539f\u56e0\uff0c\u4e0d\u8981\u4e00\u6b21\u53d1\u6563\u8fc7\u591a\u3002\n"
            u8"- \u7ed9\u51fa\u53ef\u6267\u884c\u7684\u6392\u67e5\u6b65\u9aa4\u3002\n"
            u8"- \u5982\u679c\u9700\u8981\u8865\u5145\u4fe1\u606f\uff0c\u660e\u786e\u6307\u51fa\u6700\u5173\u952e\u7684\u7f3a\u5931\u9879\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u73b0\u8c61\u89e3\u8bfb\u3001\u6839\u56e0\u5047\u8bbe\u3001\u6392\u67e5\u6b65\u9aa4\u3001\u4fee\u590d\u5efa\u8bae\u3001\u56de\u5f52\u9a8c\u8bc1\u3002"),
        QString::fromUtf8(u8"\u8f93\u51fa\u5fc5\u987b\u80fd\u6307\u5bfc\u5f00\u53d1\u8005\u9010\u6b65\u5b9a\u4f4d\u95ee\u9898\uff0c\u5e76\u660e\u786e\u6bcf\u4e00\u6b65\u8981\u89c2\u5bdf\u4ec0\u4e48\u7ed3\u679c\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"\u91cd\u6784\u8bbe\u8ba1"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u91cd\u89c6\u53ef\u7ef4\u62a4\u6027\u4e0e\u6f14\u8fdb\u80fd\u529b\u7684\u8d44\u6df1\u67b6\u6784\u5e08\uff0c\u64c5\u957f\u5bf9\u5386\u53f2\u4ee3\u7801\u8fdb\u884c\u6e10\u8fdb\u5f0f\u91cd\u6784\u3002"),
        QString::fromUtf8(u8"\u8bf7\u5728\u4e0d\u5927\u5e45\u7834\u574f\u73b0\u6709\u884c\u4e3a\u7684\u524d\u63d0\u4e0b\uff0c\u8bc4\u4f30\u4ee3\u7801\u7ed3\u6784\u3001\u804c\u8d23\u5212\u5206\u3001\u6a21\u5757\u8fb9\u754c\u548c\u6f14\u8fdb\u8def\u5f84\u3002"),
        QString::fromUtf8(
            u8"- \u4f18\u5148\u6e10\u8fdb\u5f0f\u6539\u9020\uff0c\u4e0d\u8981\u9ed8\u8ba4\u63a8\u5012\u91cd\u6765\u3002\n"
            u8"- \u517c\u987e\u6210\u672c\u3001\u98ce\u9669\u3001\u6536\u76ca\u3002\n"
            u8"- \u6307\u51fa\u54ea\u4e9b\u6b65\u9aa4\u53ef\u4ee5\u72ec\u7acb\u63d0\u4ea4\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u73b0\u72b6\u95ee\u9898\u3001\u76ee\u6807\u7ed3\u6784\u3001\u5206\u6b65\u91cd\u6784\u8ba1\u5212\u3001\u98ce\u9669\u4e0e\u6536\u76ca\u3002"),
        QString::fromUtf8(u8"\u65b9\u6848\u5e94\u9002\u5408\u771f\u5b9e\u9879\u76ee\u8fed\u4ee3\u6267\u884c\uff0c\u5e76\u80fd\u62c6\u5206\u6210\u591a\u4e2a\u53ef\u9a8c\u8bc1\u7684\u5c0f\u6b65\u9aa4\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"\u6027\u80fd\u4f18\u5316"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u64c5\u957f C++/Qt \u7a0b\u5e8f\u6027\u80fd\u5206\u6790\u4e0e\u4f18\u5316\u7684\u5de5\u7a0b\u5e08\uff0c\u5173\u6ce8 CPU\u3001\u5185\u5b58\u3001IO \u548c\u754c\u9762\u54cd\u5e94\u3002"),
        QString::fromUtf8(u8"\u8bf7\u57fa\u4e8e\u7ed9\u5b9a\u4ee3\u7801\u6216\u73b0\u8c61\uff0c\u5224\u65ad\u53ef\u80fd\u7684\u6027\u80fd\u74f6\u9888\uff0c\u5e76\u63d0\u4f9b\u53ef\u9a8c\u8bc1\u7684\u4f18\u5316\u5efa\u8bae\u3002"),
        QString::fromUtf8(
            u8"- \u5148\u505a\u6d4b\u91cf\u548c\u5b9a\u4f4d\uff0c\u518d\u8c08\u4f18\u5316\u3002\n"
            u8"- \u533a\u5206\u9ad8\u6536\u76ca\u4f18\u5316\u548c\u4f4e\u6536\u76ca\u4f18\u5316\u3002\n"
            u8"- \u4e0d\u8981\u4e3a\u4e86\u5fae\u5c0f\u6536\u76ca\u727a\u7272\u4ee3\u7801\u53ef\u7ef4\u62a4\u6027\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u74f6\u9888\u731c\u6d4b\u3001\u6d4b\u91cf\u65b9\u6848\u3001\u4f18\u5316\u70b9\u3001\u9884\u671f\u6536\u76ca\u3001\u6ce8\u610f\u4e8b\u9879\u3002"),
        QString::fromUtf8(u8"\u5efa\u8bae\u5fc5\u987b\u53ef\u64cd\u4f5c\uff0c\u5e76\u8bf4\u660e\u5982\u4f55\u9a8c\u8bc1\u4f18\u5316\u524d\u540e\u5dee\u5f02\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"\u5355\u5143\u6d4b\u8bd5"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u6ce8\u91cd\u8986\u76d6\u5173\u952e\u8def\u5f84\u548c\u8fb9\u754c\u6761\u4ef6\u7684\u6d4b\u8bd5\u5de5\u7a0b\u5e08\uff0c\u64c5\u957f\u4e3a C++/Qt \u4ee3\u7801\u8bbe\u8ba1\u6d4b\u8bd5\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6839\u636e\u7ed9\u5b9a\u51fd\u6570\u3001\u7c7b\u6216\u6a21\u5757\u884c\u4e3a\uff0c\u8bbe\u8ba1\u5408\u7406\u7684\u6d4b\u8bd5\u7b56\u7565\u4e0e\u6d4b\u8bd5\u7528\u4f8b\u3002"),
        QString::fromUtf8(
            u8"- \u91cd\u70b9\u8986\u76d6\u6838\u5fc3\u903b\u8f91\u3001\u5f02\u5e38\u8def\u5f84\u548c\u8fb9\u754c\u8f93\u5165\u3002\n"
            u8"- \u6d4b\u8bd5\u5e94\u5c3d\u91cf\u7a33\u5b9a\u3001\u53ef\u91cd\u590d\u3002\n"
            u8"- \u5982\u4e0d\u9002\u5408\u5355\u5143\u6d4b\u8bd5\uff0c\u8981\u8bf4\u660e\u539f\u56e0\u5e76\u7ed9\u51fa\u66ff\u4ee3\u65b9\u6848\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u6d4b\u8bd5\u76ee\u6807\u3001\u6d4b\u8bd5\u573a\u666f\u3001\u793a\u4f8b\u7528\u4f8b\u3001Mock/\u6869\u5efa\u8bae\u3001\u9057\u6f0f\u98ce\u9669\u3002"),
        QString::fromUtf8(u8"\u6d4b\u8bd5\u65b9\u6848\u5e94\u80fd\u76f4\u63a5\u8f6c\u6210\u6d4b\u8bd5\u4ee3\u7801\uff0c\u5e76\u4f53\u73b0\u6b63\u5e38\u6d41\u3001\u5f02\u5e38\u6d41\u548c\u8fb9\u754c\u6d41\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"\u9700\u6c42\u62c6\u89e3"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u7ecf\u9a8c\u4e30\u5bcc\u7684\u6280\u672f\u8d1f\u8d23\u4eba\uff0c\u64c5\u957f\u628a\u6a21\u7cca\u9700\u6c42\u62c6\u89e3\u6210\u6e05\u6670\u3001\u53ef\u6267\u884c\u7684\u5f00\u53d1\u4efb\u52a1\u3002"),
        QString::fromUtf8(u8"\u7528\u6237\u4f1a\u7ed9\u51fa\u4e00\u4e2a\u529f\u80fd\u60f3\u6cd5\u6216\u4e1a\u52a1\u9700\u6c42\uff0c\u4f60\u9700\u8981\u4ece\u5f00\u53d1\u843d\u5730\u89d2\u5ea6\u5b8c\u6210\u5206\u6790\u4e0e\u4efb\u52a1\u5206\u89e3\u3002"),
        QString::fromUtf8(
            u8"- \u5148\u6f84\u6e05\u76ee\u6807\uff0c\u518d\u62c6\u89e3\u4efb\u52a1\u3002\n"
            u8"- \u660e\u786e\u8f93\u5165\u3001\u8f93\u51fa\u3001\u4ea4\u4e92\u3001\u5f02\u5e38\u60c5\u51b5\u3002\n"
            u8"- \u6807\u51fa\u4f9d\u8d56\u9879\u548c\u53ef\u80fd\u7684\u5b9e\u73b0\u98ce\u9669\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u9700\u6c42\u7406\u89e3\u3001\u529f\u80fd\u62c6\u5206\u3001\u5f00\u53d1\u4efb\u52a1\u5217\u8868\u3001\u98ce\u9669\u70b9\u3001\u9a8c\u6536\u6807\u51c6\u3002"),
        QString::fromUtf8(u8"\u8f93\u51fa\u5e94\u8db3\u591f\u6e05\u6670\uff0c\u4f7f\u5f00\u53d1\u8005\u53ef\u4ee5\u76f4\u63a5\u6309\u4efb\u52a1\u5217\u8868\u63a8\u8fdb\u5b9e\u73b0\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"\u6587\u6863\u6574\u7406"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u6280\u672f\u6587\u6863\u5de5\u7a0b\u5e08\uff0c\u64c5\u957f\u628a\u96f6\u6563\u4fe1\u606f\u6574\u7406\u6210\u6e05\u6670\u3001\u4e13\u4e1a\u3001\u4fbf\u4e8e\u56e2\u961f\u534f\u4f5c\u7684\u6587\u6863\u3002"),
        QString::fromUtf8(u8"\u8bf7\u5c06\u7ed9\u5b9a\u5185\u5bb9\u6574\u7406\u4e3a\u5f00\u53d1\u6587\u6863\u3001\u8bf4\u660e\u6587\u6863\u3001\u64cd\u4f5c\u624b\u518c\u6216\u95ee\u9898\u8bb0\u5f55\u3002"),
        QString::fromUtf8(
            u8"- \u4fdd\u6301\u672f\u8bed\u7edf\u4e00\u3002\n"
            u8"- \u4f18\u5148\u7ed3\u6784\u6e05\u6670\u3001\u4fbf\u4e8e\u68c0\u7d22\u3002\n"
            u8"- \u5982\u539f\u59cb\u4fe1\u606f\u4e0d\u5b8c\u6574\uff0c\u8981\u663e\u5f0f\u6807\u6ce8\u5f85\u8865\u5145\u9879\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u6807\u9898\u3001\u80cc\u666f\u3001\u6b63\u6587\u5206\u8282\u3001\u6ce8\u610f\u4e8b\u9879\u3001\u5f85\u8865\u5145\u9879\u3002"),
        QString::fromUtf8(u8"\u6574\u7406\u540e\u7684\u5185\u5bb9\u5e94\u9002\u5408\u76f4\u63a5\u653e\u5165\u9879\u76ee\u6587\u6863\u5e93\u6216\u53d1\u7ed9\u56e2\u961f\u6210\u5458\u9605\u8bfb\u3002")));

    v.append(makePreset(
        QString::fromUtf8(u8"SQL / \u6570\u636e\u5e93"),
        QString::fromUtf8(u8"\u4f60\u662f\u4e00\u540d\u719f\u6089 SQL\u3001\u7d22\u5f15\u8bbe\u8ba1\u3001\u67e5\u8be2\u4f18\u5316\u548c\u4e1a\u52a1\u6570\u636e\u5efa\u6a21\u7684\u6570\u636e\u5e93\u5de5\u7a0b\u5e08\u3002"),
        QString::fromUtf8(u8"\u8bf7\u56f4\u7ed5\u7ed9\u5b9a\u7684\u8868\u7ed3\u6784\u3001\u67e5\u8be2\u8bed\u53e5\u6216\u4e1a\u52a1\u9700\u6c42\uff0c\u63d0\u4f9b\u6570\u636e\u5e93\u5c42\u9762\u7684\u5b9e\u73b0\u4e0e\u4f18\u5316\u5efa\u8bae\u3002"),
        QString::fromUtf8(
            u8"- \u4f18\u5148\u4fdd\u8bc1\u6b63\u786e\u6027\u548c\u6570\u636e\u4e00\u81f4\u6027\u3002\n"
            u8"- \u5bf9\u6f5c\u5728\u6027\u80fd\u95ee\u9898\u7ed9\u51fa\u89e3\u91ca\u3002\n"
            u8"- \u4fee\u6539\u8868\u7ed3\u6784\u65f6\u8bf4\u660e\u8fc1\u79fb\u98ce\u9669\u3002"),
        QString::fromUtf8(u8"\u8bf7\u6309\u4ee5\u4e0b\u7ed3\u6784\u8f93\u51fa\uff1a\u95ee\u9898\u5206\u6790\u3001SQL \u5efa\u8bae\u3001\u7d22\u5f15/\u7ed3\u6784\u5efa\u8bae\u3001\u98ce\u9669\u8bf4\u660e\u3001\u9a8c\u8bc1\u65b9\u5f0f\u3002"),
        QString::fromUtf8(u8"\u8f93\u51fa\u5e94\u517c\u987e\u4e1a\u52a1\u53ef\u7528\u6027\u4e0e\u6570\u636e\u5e93\u6027\u80fd\uff0c\u5e76\u8bf4\u660e\u53ef\u80fd\u5f71\u54cd\u7684\u73b0\u7f51\u6570\u636e\u3002")));

    return v;
}

QString promptPresetConfigPath()
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty())
        base = QCoreApplication::applicationDirPath();

    QDir dir(base);
    if (!dir.exists())
        dir.mkpath(".");
    return dir.filePath(QStringLiteral("prompt_presets.ini"));
}

bool savePresetsToConfig(const QString& path, const QVector<PromptRolePreset>& presets)
{
    CIniConfigReader reader;
    for (int i = 0; i < presets.size(); ++i)
    {
        const PromptRolePreset& preset = presets[i];
        const QString section = QStringLiteral("Preset%1").arg(i + 1, 3, 10, QLatin1Char('0'));
        reader.SetValue(section, QStringLiteral("displayName"), preset.displayName);
        reader.SetValue(section, QStringLiteral("role"), preset.role);
        reader.SetValue(section, QStringLiteral("context"), preset.context);
        reader.SetValue(section, QStringLiteral("task"), preset.task);
        reader.SetValue(section, QStringLiteral("constraints"), preset.constraints);
        reader.SetValue(section, QStringLiteral("outputFormat"), preset.outputFormat);
        reader.SetValue(section, QStringLiteral("acceptanceCriteria"), preset.acceptanceCriteria);
    }
    return reader.SaveToFile(path);
}

QVector<PromptRolePreset> loadPresetsFromConfig(const QString& path)
{
    QVector<PromptRolePreset> presets;

    CIniConfigReader reader;
    if (!reader.LoadFromFile(path))
        return presets;

    std::vector<QString> sections;
    if (!reader.GetSections(sections))
        return presets;

    for (size_t i = 0; i < sections.size(); ++i)
    {
        const QString& section = sections[i];
        if (!section.startsWith(QStringLiteral("Preset")))
            continue;

        PromptRolePreset preset;
        preset.displayName = reader.GetValue(section, QStringLiteral("displayName"), QString());
        if (preset.displayName.isEmpty())
            continue;
        preset.role = reader.GetValue(section, QStringLiteral("role"), QString());
        preset.context = reader.GetValue(section, QStringLiteral("context"), QString());
        preset.task = reader.GetValue(section, QStringLiteral("task"), QString());
        preset.constraints = reader.GetValue(section, QStringLiteral("constraints"), QString());
        preset.outputFormat = reader.GetValue(section, QStringLiteral("outputFormat"), QString());
        preset.acceptanceCriteria = reader.GetValue(section, QStringLiteral("acceptanceCriteria"), QString());
        presets.append(preset);
    }

    return presets;
}

} // namespace

CPromptComposerPage::CPromptComposerPage(QWidget* parent)
    : TabPageBase(parent)
    , m_listPresets(nullptr)
    , m_textPreview(nullptr)
    , m_btnCopy(nullptr)
    , m_splitter(nullptr)
    , m_nLeftWidth(220)
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(m_splitter);

    m_listPresets = new QListWidget(m_splitter);
    m_listPresets->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listPresets->setIconSize(QSize(24, 24));

    QWidget* right = new QWidget(m_splitter);
    QVBoxLayout* rightLay = new QVBoxLayout(right);
    rightLay->setContentsMargins(4, 4, 4, 4);
    m_textPreview = new QTextEdit(right);
    m_textPreview->setReadOnly(true);
    m_textPreview->setPlaceholderText(QString::fromUtf8(u8"\u683c\u5f0f\u5316\u540e\u7684\u63d0\u793a\u8bcd\u4f1a\u663e\u793a\u5728\u8fd9\u91cc\u3002"));
    m_btnCopy = new QPushButton(QString::fromUtf8(u8"\u590d\u5236"), right);
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(m_btnCopy);
    rightLay->addWidget(m_textPreview);
    rightLay->addLayout(btnRow);

    m_splitter->addWidget(m_listPresets);
    m_splitter->addWidget(right);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    loadPresets();
    rebuildPresetList();

    connect(m_listPresets, &QListWidget::itemClicked, this, &CPromptComposerPage::onPresetItemClicked);
    connect(m_btnCopy, &QPushButton::clicked, this, &CPromptComposerPage::onCopyClicked);
}

CPromptComposerPage::~CPromptComposerPage()
{
}

void CPromptComposerPage::loadPresets()
{
    const QString configPath = promptPresetConfigPath();
    m_presets = loadPresetsFromConfig(configPath);
    if (!m_presets.isEmpty())
        return;

    m_presets = defaultPresets();
    savePresetsToConfig(configPath, m_presets);
}

void CPromptComposerPage::rebuildPresetList()
{
    if (!m_listPresets)
        return;

    m_listPresets->clear();

    QStyle* st = style();
    static const QStyle::StandardPixmap icons[] = {
        QStyle::SP_FileDialogNewFolder,
        QStyle::SP_ComputerIcon,
        QStyle::SP_DialogYesButton,
        QStyle::SP_FileDialogDetailedView,
    };
    for (int i = 0; i < m_presets.size(); ++i)
    {
        const PromptRolePreset& pr = m_presets[i];
        QIcon icon = st->standardIcon(icons[i % 4]);
        QListWidgetItem* it = new QListWidgetItem(icon, pr.displayName);
        it->setData(Qt::UserRole, i);
        m_listPresets->addItem(it);
    }
}

void CPromptComposerPage::updateLayout(int leftWidth)
{
    int w = leftWidth;
    if (w < 160)
        w = 160;
    m_nLeftWidth = w;
    if (m_splitter)
    {
        QList<int> sizes;
        sizes << w << qMax(100, width() - w - m_splitter->handleWidth());
        m_splitter->setSizes(sizes);
    }
}

QString CPromptComposerPage::buildFormattedPrompt(const PromptRolePreset& p)
{
    QString out;
    out += QString::fromUtf8(u8"## \u89d2\u8272\n");
    out += p.role.trimmed();
    out += QString::fromUtf8(u8"\n\n## \u80cc\u666f\n");
    out += p.context.trimmed();
    out += QString::fromUtf8(u8"\n\n## \u4efb\u52a1\n");
    out += p.task.trimmed();
    out += QString::fromUtf8(u8"\n\n## \u7ea6\u675f\n");
    out += p.constraints.trimmed();
    out += QString::fromUtf8(u8"\n\n## \u8f93\u51fa\u683c\u5f0f\n");
    out += p.outputFormat.trimmed();
    out += QString::fromUtf8(u8"\n\n## \u9a8c\u6536\u6807\u51c6\n");
    out += p.acceptanceCriteria.trimmed();
    out += QLatin1Char('\n');
    return out;
}

void CPromptComposerPage::onPresetItemClicked(QListWidgetItem* item)
{
    if (!item)
        return;
    bool ok = false;
    int idx = item->data(Qt::UserRole).toInt(&ok);
    if (!ok)
        return;
    if (idx < 0 || idx >= m_presets.size())
        return;

    CEditPromptFieldsDialog dlg(this);
    dlg.setPreset(m_presets[idx]);
    if (dlg.exec() != QDialog::Accepted)
        return;

    PromptRolePreset filled = dlg.fields();
    filled.displayName = m_presets[idx].displayName;
    m_textPreview->setPlainText(buildFormattedPrompt(filled));

    if (!dlg.saveToConfigRequested())
        return;

    m_presets[idx] = filled;
    rebuildPresetList();

    const QString configPath = promptPresetConfigPath();
    if (!savePresetsToConfig(configPath, m_presets))
    {
        QMessageBox::warning(
            this,
            QString::fromUtf8(u8"\u63d0\u793a"),
            QString::fromUtf8(u8"\u66f4\u65b0\u5230\u914d\u7f6e\u6587\u4ef6\u5931\u8d25\u3002"));
        return;
    }

    QMessageBox::information(
        this,
        QString::fromUtf8(u8"\u63d0\u793a"),
        QString::fromUtf8(u8"\u5f53\u524d\u9884\u8bbe\u5df2\u66f4\u65b0\u5230\u914d\u7f6e\u6587\u4ef6\u3002"));
}

void CPromptComposerPage::onCopyClicked()
{
    const QString t = m_textPreview->toPlainText();
    if (t.isEmpty())
        return;
    QClipboard* cb = QApplication::clipboard();
    if (cb)
        cb->setText(t);
}
