#pragma once

#include <mutex>
#include <list>

#include "WalletTaskItem.h"

class WalletTask
{
public:
	WalletTask()
	{
	};
	~WalletTask()
	{
	};

	//============================//
	//һ�鷽������������Ŀ(�Ӻ���ǰ����)
	WalletTaskItem * getItemBegin()
	{
		m_lock.lock();
		if (lstItem.size() == 0)
		{
			m_lock.unlock();
			return NULL;
		}

		oIT = lstItem.begin();

		m_lock.unlock();
		return *oIT;
	};
	WalletTaskItem * getItemNext()
	{
		m_lock.lock();
		if (lstItem.size() == 0)
		{
			m_lock.unlock();
			return NULL;
		}
		if (lstItem.end() == oIT)
		{
			m_lock.unlock();
			return NULL;
		}

		oIT++;

		if (lstItem.end() == oIT)
		{
			m_lock.unlock();
			return NULL;
		}

		m_lock.unlock();
		return *oIT;
	};
	//��ȡ��һ�������������
	WalletTaskItem * getItemPending()
	{
		WalletTaskItem * pTaskOut = NULL;
		m_lock.lock();
		for (std::list<WalletTaskItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); ++oITTmp)
		{
			WalletTaskItem * pTask = *oITTmp;
			//�жϵ�ǰ�����Ƿ����ڴ�����
			if (0 == pTask->getState())
			{
				pTaskOut = *oITTmp;
				break;
			}
		}
		m_lock.unlock();

		return pTaskOut;
	};

	//����Ԫ�ظ���
	unsigned int getSize()
	{
		return (unsigned int)lstItem.size();
	};

	//����һ��Ԫ��
	void pushItem(WalletTaskItem * pItem)
	{
		m_lock.lock();
		lstItem.push_back(pItem);
		m_lock.unlock();
	};
	//ɾ��һ��Ԫ�أ�������һ������
	void popItem(WalletTaskItem * pItem)
	{
		m_lock.lock();
		for (std::list<WalletTaskItem *>::iterator oITTmp = lstItem.begin(); oITTmp != lstItem.end(); )
		{
			WalletTaskItem * pTask = *oITTmp;
			if (pItem == *oITTmp)
			{
				oIT = lstItem.erase(oITTmp);

				m_lock.unlock();
				return;
			}
			++oITTmp;
		}
		m_lock.unlock();

		return;
	};

private:
	std::mutex m_lock;
	std::list<WalletTaskItem *> lstItem;
	std::list<WalletTaskItem *>::const_iterator oIT;
};
