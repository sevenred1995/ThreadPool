#include "Thread.h"
#include "ThreadPoolExecutor.h"
int i = 0;
class Test :public Runnable {
public:
	~Test(){}
	void Run() {
		i++;
		printf("hello! %d\n",i);
	}
};
int main() {
	CThreadPoolExecutor* pEx = new CThreadPoolExecutor();
	pEx->Init(1, 10, 50);
	Test t;
	for (int i = 0; i < 100; i++)
	{
		while (!pEx->Excute(&t))
		{
		}
	}
	pEx->Terminate();
	delete pEx;
	getchar();
	return 0;
}