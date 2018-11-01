#include <windows.h>
#include <iostream>
const unsigned short SIZE_OF_BUFFER = 10; //����������
unsigned short ProductID = 0;    //��Ʒ��
unsigned short ConsumeID = 0;    //�������ĵĲ�Ʒ��
unsigned short in = 0;      //��Ʒ��������ʱ�Ļ������±�
unsigned short out = 0;      //��Ʒ��������ʱ�Ļ������±�
int g_buffer[SIZE_OF_BUFFER];    //�������Ǹ�ѭ������
bool g_continue = true;      //���Ƴ������
HANDLE g_hMutex;       //�����̼߳�Ļ���
HANDLE g_hFullSemaphore;     //����������ʱ��ʹ�����ߵȴ�
HANDLE g_hEmptySemaphore;     //����������ʱ��ʹ�����ߵȴ�
DWORD WINAPI Producer(LPVOID);    //�������߳�
DWORD WINAPI Consumer(LPVOID);    //�������߳�
int main()
{
	//�������������ź�
	g_hMutex = CreateMutex(NULL, FALSE, NULL);
	g_hFullSemaphore = CreateSemaphore(NULL, SIZE_OF_BUFFER - 1, SIZE_OF_BUFFER - 1, NULL);
	g_hEmptySemaphore = CreateSemaphore(NULL, 0, SIZE_OF_BUFFER - 1, NULL);
	//�����������ֵ�����Է��֣��������߸������������߸���ʱ��
	//�����ٶȿ죬�����߾����ȴ������ߣ���֮�������߾����ȴ� 
	const unsigned short PRODUCERS_COUNT = 3;  //�����ߵĸ���
	const unsigned short CONSUMERS_COUNT = 1;  //�����ߵĸ���
	//�ܵ��߳���
	const unsigned short THREADS_COUNT = PRODUCERS_COUNT + CONSUMERS_COUNT;
	HANDLE hThreads[PRODUCERS_COUNT]; //���̵߳�handle
	DWORD producerID[CONSUMERS_COUNT]; //�������̵߳ı�ʶ��
	DWORD consumerID[THREADS_COUNT]; //�������̵߳ı�ʶ��
	//�����������߳�
	for (int i = 0; i<PRODUCERS_COUNT; ++i){
		hThreads[i] = CreateThread(NULL, 0, Producer, NULL, 0, &producerID[i]);
		if (hThreads[i] == NULL) return -1;
	}
	//�����������߳�
	for (int i1 = 0; i<CONSUMERS_COUNT; ++i1){
		hThreads[PRODUCERS_COUNT + i1] = CreateThread(NULL, 0, Consumer, NULL, 0, &consumerID[i1]);
		if (hThreads[i1] == NULL) return -1;
	}
	while (g_continue){
		if (getchar()){ //���س�����ֹ��������
			g_continue = false;
		}
	}
	return 0;
}
//����һ����Ʒ����ģ����һ�£�������²�Ʒ��ID��
void Produce()
{
	std::cerr << "Producing " << ++ProductID << " ... ";
	std::cerr << "Succeed" << std::endl;
}
//���������Ĳ�Ʒ���뻺����
void Append()
{
	std::cerr << "Appending a product ... ";
	g_buffer[in] = ProductID;
	in = (in + 1) % SIZE_OF_BUFFER;
	std::cerr << "Succeed" << std::endl;
	//�����������ǰ��״̬
	for (int i = 0; i<SIZE_OF_BUFFER; ++i){
		std::cout << i << ": " << g_buffer[i];
		if (i == in) std::cout << " <-- ����";
		if (i == out) std::cout << " <-- ����";
		std::cout << std::endl;
	}
}
//�ӻ�������ȡ��һ����Ʒ
void Take()
{
	std::cerr << "Taking a product ... ";
	ConsumeID = g_buffer[out];
	out = (out + 1) % SIZE_OF_BUFFER;
	std::cerr << "Succeed" << std::endl;
	//�����������ǰ��״̬
	for (int i = 0; i<SIZE_OF_BUFFER; ++i){
		std::cout << i << ": " << g_buffer[i];
		if (i == in) std::cout << " <-- ����";
		if (i == out) std::cout << " <-- ����";
		std::cout << std::endl;
	}
}
//����һ����Ʒ
void Consume()
{
	std::cerr << "Consuming " << ConsumeID << " ... ";
	std::cerr << "Succeed" << std::endl;
}
//������
DWORD  WINAPI Producer(LPVOID lpPara)
{
	while (g_continue){
		WaitForSingleObject(g_hFullSemaphore, INFINITE);
		WaitForSingleObject(g_hMutex, INFINITE);
		Produce();
		Append();
		Sleep(1500);
		ReleaseMutex(g_hMutex);
		ReleaseSemaphore(g_hEmptySemaphore, 1, NULL);
	}
	return 0;
}
//������
DWORD  WINAPI Consumer(LPVOID lpPara)
{
	while (g_continue){
		WaitForSingleObject(g_hEmptySemaphore, INFINITE);
		WaitForSingleObject(g_hMutex, INFINITE);
		Take();
		Consume();
		Sleep(1500);
		ReleaseMutex(g_hMutex);
		ReleaseSemaphore(g_hFullSemaphore, 1, NULL);
	}
	return 0;
}
