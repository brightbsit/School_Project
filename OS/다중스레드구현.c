#include <windows.h>
#include <stdio.h>
#include <stdlib.h> // rand()�Լ� ���� ���̺귯��
#include <math.h> // pow(), sqrt()
#include <time.h> //�ð������� ���� ���̺귯��

#define MAX 40000000
DWORD count = 0;
DWORD Multi_count[4] = { 0 };

//���ڷ� �Ѱ��� ������ �����ϱ� ���� ����ü
typedef struct Mydata {
	int val1;
	int val2;
	int num;
}Mydata, * pMydata;

// �̱� ������
DWORD WINAPI SingleResult(void* Param) {
	double x, y, i;
	double spot;
	pMydata ptmp = (pMydata)Param;
	srand(time(NULL));
	for (i = ptmp->val1; i < ptmp->val2; i++) {
		x = (double)rand() / RAND_MAX;
		y = (double)rand() / RAND_MAX;
		spot = sqrt(pow(x, 2) + pow(y, 2));
		if (spot < 1) count = count + 1;
	}
	return 0;
}

// ��Ƽ ������
/*
	�⺻������ �̱� ������� �ڵ� �帧�� ����
	�������� �� �����庰�� count���� ���� ó���ϰ�,
	���� �����ϴ� �迭�� �� �ٴ� ������ ���̰� ����.
*/
DWORD WINAPI MultieResult(void* Param) {
	double x, y, i;
	double spot;
	pMydata ptmp = (pMydata)Param;
	srand(time(NULL));
	for (i = ptmp->val1; i < ptmp->val2; i++) {
		x = (double)rand() / RAND_MAX;
		y = (double)rand() / RAND_MAX;
		spot = (int)sqrt(pow(x, 2) + pow(y, 2));
		if (spot < 1)
			switch (ptmp->num) {
			case 0:
				Multi_count[0]++;
				break;
			case 1:
				Multi_count[1]++;
				break;
			case 2:
				Multi_count[2]++;
				break;
			case 3:
				Multi_count[3]++;
				break;
			}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	time_t start, finish, start2, finish2; // �ð� ������ ���� ������
	double duration, duration2, result, result2;
	DWORD ThreadId, ThreadArray[4]; //������ ���̵� / ���̵� �迭
	HANDLE ThreadHandle, ThreadArrayHandle[4]; //�ڵ鷯
	Mydata SingleMydata, MulMydata[4]; // ���ڷ� �Ѱ��� ����ü ����
	int i, j, first, end, count_sum = 0;


	start = time(NULL);
	/*���� ������ ���� ����*/
	SingleMydata.val1 = 0;
	SingleMydata.val2 = MAX;
	SingleMydata.num = 1;

	/* ���� ������ */
	ThreadHandle = CreateThread(NULL, 0, SingleResult, &SingleMydata, 0, &ThreadId); // ���� ������ ����
	WaitForSingleObject(ThreadHandle, INFINITE); // �ڽ� �����尡 �����⸦ ��ٸ�
	CloseHandle(ThreadHandle);

	finish = time(NULL);
	duration = finish - start;
	result = (double)(4 * count) / MAX;

	printf("PI�� = %f\n", result);
	printf("���� ������ �ɸ� �ð� = %.2f���Դϴ�\n", duration);


	/* ��Ƽ ������ */
	start2 = time(NULL);
	for (i = 0, first = 0, end = 10000000; i < 4; i++, first = end, end += 10000000) {
		MulMydata[i].val1 = first;
		MulMydata[i].val2 = end;
		MulMydata[i].num = i;
		ThreadArrayHandle[i] = CreateThread(NULL, 0, MultieResult, &MulMydata[i], 0, &ThreadArray[i]);
	}
	WaitForMultipleObjects(4, ThreadArrayHandle, TRUE, INFINITE); // �ڽ� �����尡 �����⸦ ��ٸ�
	CloseHandle(&ThreadArrayHandle);

	finish2 = time(NULL);
	duration2 = (double)(finish2 - start2);

	for (j = 0; j < 4; j++) {
		count_sum += Multi_count[j];
	}
	printf("%d %d\n", count_sum, count);

	result2 = (double)(count_sum * 4) / MAX;

	printf("PI �� = %f\n", result2);
	printf("��Ƽ ������ �ɸ� �ð� = %.2f���Դϴ�\n", duration2);

	return 0;
}

