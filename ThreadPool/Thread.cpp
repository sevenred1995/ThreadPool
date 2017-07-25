#include "Thread.h"
CThread::CThread():
	m_pRun(NULL),
	m_bRun(false)
{
}

CThread::CThread(Runnable * pRun):
	m_ThreadName(""),
	m_pRun(pRun),
	m_bRun(false)
{
}

CThread::CThread(const char * threadName, Runnable * pRun):
	m_ThreadName(threadName),
	m_pRun(pRun),
	m_bRun(false)
{
}

CThread::CThread(std::string threadName, Runnable * pRun):
	m_ThreadName(threadName),
	m_pRun(pRun),
	m_bRun(false)
{
}

CThread::~CThread()
{
}

bool CThread::Start(bool bSuspend)
{
	if (m_bRun)
		return true;
	if (bSuspend)
		m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, CREATE_SUSPENDED, &m_ThreadID);
	else
		m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, 0, &m_ThreadID);
	m_bRun = (NULL != m_handle);
	return m_bRun;
}

void CThread::Run()
{
	if (!m_bRun)
		return;
	if (NULL != m_pRun)
		m_pRun->Run();
	m_bRun = false;
}

void CThread::Join(int timeOut)
{
	if (NULL == m_handle || !m_bRun)
		return;
	if (timeOut <= 0)
		timeOut = INFINITE;
	::WaitForSingleObject(m_handle, timeOut);
}

void CThread::Resume()
{
	if (NULL == m_handle || !m_bRun)
		return;
	::ResumeThread(m_handle);
}

void CThread::Suspend()
{
	if (NULL == m_handle || !m_bRun)
		return;
	::SuspendThread(m_handle);
}

bool CThread::Terminate(unsigned long exitCode)
{
	if (NULL == m_handle || !m_bRun)
		return true;
	if (::TerminateThread(m_handle, exitCode))
	{
		CloseHandle(m_handle);
		return true;
	}
	return false;
}

unsigned int CThread::GetThreadID()
{
	return m_ThreadID;
}

std::string CThread::GetThreadName()
{
	return m_ThreadName;
}

void CThread::SetThreadName(std::string threadName)
{
	m_ThreadName = threadName;
}

void CThread::SetThreadName(const char * threadName)
{
	m_ThreadName = threadName;
}

unsigned int CThread::StaticThreadFunc(void * arg)
{
	CThread* pThread = (CThread *)arg;
	pThread->Run();
	return 0;
}
