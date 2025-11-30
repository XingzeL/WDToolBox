// Observer.cpp: 观察者模式实现
//
#include "pch.h"
#include "Observer.h"
#include <algorithm>

CObservable::CObservable()
{
}

CObservable::~CObservable()
{
	m_vecObservers.clear();
}

void CObservable::AddObserver(IObserver* pObserver)
{
	if (pObserver == nullptr)
		return;

	// 检查是否已存在
	auto it = std::find(m_vecObservers.begin(), m_vecObservers.end(), pObserver);
	if (it == m_vecObservers.end())
	{
		m_vecObservers.push_back(pObserver);
	}
}

void CObservable::RemoveObserver(IObserver* pObserver)
{
	if (pObserver == nullptr)
		return;

	auto it = std::find(m_vecObservers.begin(), m_vecObservers.end(), pObserver);
	if (it != m_vecObservers.end())
	{
		m_vecObservers.erase(it);
	}
}

void CObservable::NotifyObservers(const CString& strEventType, void* pData)
{
	// 通知所有观察者
	for (IObserver* pObserver : m_vecObservers)
	{
		if (pObserver != nullptr)
		{
			pObserver->OnDataChanged(strEventType, pData);
		}
	}
}

