#include "Thread.h"
#include "ThreadPoolExecutor.h"
class Test :public Runnable {
public:
	~Test(){}
	void Run() {
		printf("hello!\n");
	}
};
int main() {
	CThreadPoolExecutor* pEx = new CThreadPoolExecutor();
	pEx->Init(1, 10, 40);
	Test t;
	for (int i = 0; i < 50; i++)
	{
		//while (!pEx->Excute(&t))
		//{
		//}
		pEx->Excute(&t);
	}
	pEx->Terminate();
	delete pEx;
	getchar();
	return 0;
}