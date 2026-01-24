#pragma once
#include <QWidget>

// 分页基类接口
// 定义所有分页的通用接口
class TabPageBase : public QWidget
{
    Q_OBJECT

public:
    explicit TabPageBase(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~TabPageBase() {}

    // 更新布局（窗口大小改变时调用）
    virtual void updateLayout(int leftWidth) = 0;

    // 获取当前左侧宽度
    virtual int getLeftWidth() const = 0;

    // 设置左侧宽度
    virtual void setLeftWidth(int width) = 0;
};
