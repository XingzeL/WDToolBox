#pragma once

#include <QIcon>
#include <QMetaType>
#include <QString>

// Tool metadata used by UI and manager.
struct ToolInfo
{
    QString name;
    QString path;
    QString category;
    QIcon icon;
    int iconIndex;
    bool highlighted;

    ToolInfo()
        : iconIndex(-1)
        , highlighted(false)
    {
    }
};

Q_DECLARE_METATYPE(ToolInfo)
