#pragma once
#include "Thread.h"
#include <list>
#include <set>
class CThreadPoolExecutor
{
public:
	CThreadPoolExecutor();
	~CThreadPoolExecutor();

	bool Init(unsigned int minThreads, unsigned int maxThreads, unsigned int maxPendingTasks);
	bool Excute(Runnable* pRunnable);
	void Terminate();
	unsigned int GetThreadPoolSize();

private: 
	Runnable* GetTask();
	static unsigned int _stdcall StaticThreadFunc(void* arg);

private:
	class CWorker :public CThread
	{
	public:
		CWorker(CThreadPoolExecutor* pThreadPool, Runnable* pFirstTask = NULL);
		~CWorker();
		void Run();
	private: 
		CThreadPoolExecutor* m_pThreadPool;
		Runnable*            m_pFirstTask;
		volatile     bool    m_bRun;
	};

	typedef std::set<CWorker*>   ThreadPool;
	typedef std::list<Runnable*> Tasks;
	typedef Tasks::iterator      TasksItr;
	typedef ThreadPool::iterator ThreadPoolItr;


	ThreadPool m_ThreadPool;
	ThreadPool m_TrashThread;
	Tasks      m_Tasks;

	CRITICAL_SECTION m_csTasksLock;
	CRITICAL_SECTION m_csThreadPoolLock;

	volatile bool m_bRun;
	volatile bool m_bInsertTask;
	volatile unsigned int m_minThreads;
	volatile unsigned int m_maxThreads;
	volatile unsigned int m_maxPendingTasks;
};

