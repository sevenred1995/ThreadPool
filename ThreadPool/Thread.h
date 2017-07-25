#pragma once
#include <windows.h>
#include <process.h>
#include <string>

class Runnable{
	public:
		virtual ~Runnable(){}
		virtual void Run() = 0;
};

class CThread:public Runnable
{
private:
	explicit CThread(const CThread& rhs);
public:
	CThread();
	CThread(Runnable* pRun);
	CThread(const char* threadName, Runnable* pRun = NULL);
	CThread(std::string threadName, Runnable* pRun = NULL);
	~CThread();

	bool Start(bool bSuspend=false);
	virtual void Run();
	void Join(int timeOut = 1);
	void Resume();
	void Suspend();
	bool Terminate(unsigned long exitCode);

	unsigned int GetThreadID();
	std::string GetThreadName();
	void SetThreadName(std::string threadName);
	void SetThreadName(const char* threadName);
private:
	static unsigned int _stdcall StaticThreadFunc(void* arg);
private:
	HANDLE          m_handle;
	Runnable* const m_pRun;
	unsigned int    m_ThreadID;
	std::string     m_ThreadName;
	volatile bool   m_bRun;
};

