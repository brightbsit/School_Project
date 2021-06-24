#include <windows.h>
#include <stdio.h>
#include <stdlib.h> // rand()함수 포함 라이브러리
#include <math.h> // pow(), sqrt()
#include <time.h> //시간측정을 위한 라이브러리

#define MAX 40000000
DWORD count = 0;
DWORD Multi_count[4] = { 0 };

//인자로 넘겨줄 정보를 저장하기 위한 구조체
typedef struct Mydata {
	int val1;
	int val2;
	int num;
}Mydata, * pMydata;

// 싱글 스레드
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

// 멀티 스레드
/*
	기본적으로 싱글 스레드와 코드 흐름은 같음
	마지막에 각 스레드별로 count값을 따로 처리하고,
	각각 저장하는 배열을 둔 다는 점에서 차이가 있음.
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
	time_t start, finish, start2, finish2; // 시간 측정을 위한 변수들
	double duration, duration2, result, result2;
	DWORD ThreadId, ThreadArray[4]; //스레드 아이디 / 아이디 배열
	HANDLE ThreadHandle, ThreadArrayHandle[4]; //핸들러
	Mydata SingleMydata, MulMydata[4]; // 인자로 넘겨줄 구조체 변수
	int i, j, first, end, count_sum = 0;


	start = time(NULL);
	/*단일 스레드 인자 설정*/
	SingleMydata.val1 = 0;
	SingleMydata.val2 = MAX;
	SingleMydata.num = 1;

	/* 단일 스레드 */
	ThreadHandle = CreateThread(NULL, 0, SingleResult, &SingleMydata, 0, &ThreadId); // 단일 스레드 생성
	WaitForSingleObject(ThreadHandle, INFINITE); // 자식 스레드가 끝나기를 기다림
	CloseHandle(ThreadHandle);

	finish = time(NULL);
	duration = finish - start;
	result = (double)(4 * count) / MAX;

	printf("PI값 = %f\n", result);
	printf("단일 스레드 걸린 시간 = %.2f초입니다\n", duration);


	/* 멀티 스레드 */
	start2 = time(NULL);
	for (i = 0, first = 0, end = 10000000; i < 4; i++, first = end, end += 10000000) {
		MulMydata[i].val1 = first;
		MulMydata[i].val2 = end;
		MulMydata[i].num = i;
		ThreadArrayHandle[i] = CreateThread(NULL, 0, MultieResult, &MulMydata[i], 0, &ThreadArray[i]);
	}
	WaitForMultipleObjects(4, ThreadArrayHandle, TRUE, INFINITE); // 자식 스레드가 끝나기를 기다림
	CloseHandle(&ThreadArrayHandle);

	finish2 = time(NULL);
	duration2 = (double)(finish2 - start2);

	for (j = 0; j < 4; j++) {
		count_sum += Multi_count[j];
	}
	printf("%d %d\n", count_sum, count);

	result2 = (double)(count_sum * 4) / MAX;

	printf("PI 값 = %f\n", result2);
	printf("멀티 스레드 걸린 시간 = %.2f초입니다\n", duration2);

	return 0;
}

