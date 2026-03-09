#pragma once
#include <QString>
#include <QIcon>
#include <QMetaType>

// ToolInfo.h: 工具信息结构定义

// 工具信息
struct ToolInfo
{
    QString name;      // 工具名称
    QString path;      // 工具路径
    QString category;  // 所属分类
    QIcon icon;        // 工具图标
    int iconIndex;     // 图标索引（兼容旧逻辑）
    bool highlighted;  // 是否高亮显示

    ToolInfo()
        : iconIndex(-1)
        , highlighted(false)
    {
    }
};

// 让 ToolInfo 可用于 Qt 元对象系统（如 QVariant）
Q_DECLARE_METATYPE(ToolInfo)
