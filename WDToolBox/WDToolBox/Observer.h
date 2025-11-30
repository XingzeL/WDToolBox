// Observer.h: 观察者模式接口
//
#pragma once
#include <vector>

// 观察者接口
// 用于接收数据变化的通知
class IObserver
{
public:
	virtual ~IObserver() {}

	// 数据更新通知
	// strEventType: 事件类型（如 "ToolAdded", "CategoryChanged" 等）
	// pData: 事件数据（可选）
	virtual void OnDataChanged(const CString& strEventType, void* pData = nullptr) = 0;
};

// 可观察对象接口
// 管理观察者列表，提供订阅和取消订阅功能
class IObservable
{
public:
	virtual ~IObservable() {}

	// 添加观察者
	virtual void AddObserver(IObserver* pObserver) = 0;

	// 移除观察者
	virtual void RemoveObserver(IObserver* pObserver) = 0;

	// 通知所有观察者
	virtual void NotifyObservers(const CString& strEventType, void* pData = nullptr) = 0;
};

// 可观察对象基类实现
// 提供观察者管理的默认实现
class CObservable : public IObservable
{
public:
	CObservable();
	virtual ~CObservable();

	// IObservable 接口实现
	virtual void AddObserver(IObserver* pObserver) override;
	virtual void RemoveObserver(IObserver* pObserver) override;
	virtual void NotifyObservers(const CString& strEventType, void* pData = nullptr) override;

protected:
	// 观察者列表
	std::vector<IObserver*> m_vecObservers;
};

