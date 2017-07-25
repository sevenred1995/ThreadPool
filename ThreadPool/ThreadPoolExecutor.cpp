#include "ThreadPoolExecutor.h"

CThreadPoolExecutor::CThreadPoolExecutor():
	m_bRun(false),m_bInsertTask(false)
{
	InitializeCriticalSection(&m_csTasksLock);
	InitializeCriticalSection(&m_csThreadPoolLock);
}

CThreadPoolExecutor::~CThreadPoolExecutor()
{
	Terminate();
	DeleteCriticalSection(&m_csTasksLock);
	DeleteCriticalSection(&m_csThreadPoolLock);
}

bool CThreadPoolExecutor::Init(unsigned int minThreads, unsigned int maxThreads, unsigned int maxPendingTasks)
{
	if (minThreads == 0)
		return false;
	if (maxThreads < minThreads)
		return false;
	m_minThreads = minThreads;
	m_maxThreads = maxThreads;
	m_maxPendingTasks = maxPendingTasks;
	unsigned int i = m_ThreadPool.size();
	for (;i<minThreads;i++)
	{
		CWorker* pWorker = new CWorker(this);
		if (NULL == pWorker)
			return false;
		EnterCriticalSection(&m_csThreadPoolLock);
		m_ThreadPool.insert(pWorker);
		LeaveCriticalSection(&m_csThreadPoolLock);
		pWorker->Start();
	}
	m_bRun = true;
	m_bInsertTask = true;
	return true;
}

bool CThreadPoolExecutor::Excute(Runnable * pRunnable)
{
	if (!m_bInsertTask)
		return false;
	if (NULL == pRunnable)
		return false;
	if (m_Tasks.size() >= m_maxPendingTasks)
	{
		if (m_ThreadPool.size() < m_maxThreads)
		{
			CWorker* pWorker = new CWorker(this, pRunnable);
			if (NULL == pWorker)
				return false;
			EnterCriticalSection(&m_csThreadPoolLock);
			m_ThreadPool.insert(pWorker);
			LeaveCriticalSection(&m_csThreadPoolLock);
			pWorker->Start();
		}
		else
			return false;
	}
	else
	{
		EnterCriticalSection(&m_csTasksLock);
		m_Tasks.push_back(pRunnable);
		LeaveCriticalSection(&m_csTasksLock);
	}
	return true;
}

void CThreadPoolExecutor::Terminate()
{
	m_bInsertTask = false;
	while (m_Tasks.size()>0)
	{
		Sleep(1);
	}
	m_bRun = false;
	m_minThreads = 0;
	m_maxThreads = 0;
	m_maxPendingTasks = 0;
	while (m_ThreadPool.size()>0)
	{
		Sleep(1);
	}
	EnterCriticalSection(&m_csThreadPoolLock);
	ThreadPoolItr itr = m_TrashThread.begin();
	while (itr != m_TrashThread.end())
	{
		(*itr)->Join();
		delete *itr;
		m_TrashThread.erase(itr);
		itr = m_TrashThread.begin();
	}
	LeaveCriticalSection(&m_csThreadPoolLock);
}

unsigned int CThreadPoolExecutor::GetThreadPoolSize()
{
	return m_ThreadPool.size();
}

Runnable * CThreadPoolExecutor::GetTask()
{
	Runnable* Task = NULL;
	EnterCriticalSection(&m_csTasksLock);
	if (!m_TrashThread.empty()) {
		Task = m_Tasks.front();
		m_Tasks.pop_front();
	}
	LeaveCriticalSection(&m_csTasksLock);
	return Task;
}

unsigned int CThreadPoolExecutor::StaticThreadFunc(void * arg)
{
	return 0;
}

CThreadPoolExecutor::CWorker::CWorker(CThreadPoolExecutor * pThreadPool, Runnable * pFirstTask):
	m_pThreadPool(pThreadPool),m_pFirstTask(pFirstTask),m_bRun(true)
{
}

CThreadPoolExecutor::CWorker::~CWorker()
{
}

void CThreadPoolExecutor::CWorker::Run()
{
	Runnable* pTask = NULL;
	while (m_bRun)
	{
		if (NULL == m_pFirstTask)
			pTask = m_pThreadPool->GetTask();
		else
		{
			pTask = m_pFirstTask;
			m_pFirstTask = NULL;
		}
		if (NULL == pTask)
		{
			EnterCriticalSection(&(m_pThreadPool->m_csThreadPoolLock));
			if (m_pThreadPool->GetThreadPoolSize() > m_pThreadPool->m_minThreads)
			{
				ThreadPoolItr itr = m_pThreadPool->m_ThreadPool.find(this);
				if (itr != m_pThreadPool->m_ThreadPool.end())
				{
					m_pThreadPool->m_ThreadPool.erase(itr);
					m_pThreadPool->m_TrashThread.insert(this);
				}
				m_bRun = false;
			}
			else
			{
				ThreadPoolItr itr = m_pThreadPool->m_TrashThread.begin();
				while (itr!=m_pThreadPool->m_TrashThread.end())
				{
					(*itr)->Join();
					delete (*itr);
					m_pThreadPool->m_TrashThread.erase(itr);
					itr = m_pThreadPool->m_TrashThread.begin();
				}
			}
			LeaveCriticalSection(&(m_pThreadPool->m_csThreadPoolLock));
			continue;
		}
		else
		{
			pTask->Run();
			pTask = NULL;
		}
	}
}
