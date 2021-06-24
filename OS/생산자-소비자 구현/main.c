#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
int x = 0;
int count = 0;
int front = 0;
int end = 0;
buffer_item buffer[BUFFER_SIZE];

pthread_mutex_t mutex;
pthread_attr_t attr;
sem_t full, empty;

int insert_item(buffer_item item) {
	sem_wait(&empty);
	pthread_mutex_lock(&mutex); // lock
	// 버퍼가 꽉 차 있는지 확인
	if (count == BUFFER_SIZE) {
		return -1;
	}
	else {
		buffer[front] = item;
		front = (front + 1) % BUFFER_SIZE;
		count++;
	}
	pthread_mutex_unlock(&mutex); // lock해제 
	sem_post(&full); // 세마포의 잠금 해제 
	return 0;
}

int remove_item(buffer_item* item) {
	sem_wait(&full);
	pthread_mutex_lock(&mutex); // lock
	// 버퍼가 비어있는지 확인
	if (count == 0) {
		return -1;
	}
	else {
		*item = buffer[end];
		end = (end + 1) % BUFFER_SIZE;
		count--;
	}
	pthread_mutex_unlock(&mutex); // lock해제 
	sem_post(&empty); // 세마포의 잠금 해제 
	return 0;
}

void initBuffer() {
	pthread_mutex_init(&mutex, NULL); // mutex 초기화
	sem_init(&full, 0, 5);
	sem_init(&empty, 0, BUFFER_SIZE);
}

void* producer(void* param) {
	buffer_item item;
	srand(x);
	x++;
	while (1) {
		item = rand();
		if (insert_item(item)) {
			printf("report error\n");
		}
		else {
			printf("producer produced %d \n", item);
		}
		sleep(rand()%2 + 1);
	}
}

void* consumer(void* param) {
	buffer_item item;
	while (1) {
		if (remove_item(&item)) {
			printf("report error condition\n");
		}
		else {
			printf("consumer consumed %d \n", item);
		}
		sleep(rand() % 2 + 1);
	}
}

int main(int argc, char* argv[]) {
	int i, j;
	int num1 = atoi(argv[1]);
	int num2 = atoi(argv[2]);
	int num3 = atoi(argv[3]);

	int s = num2 + num3;

	pthread_t tid;

	initBuffer();

	for (i = 0; i < num2; i++) {
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, producer, NULL);
	}

	for (j = 0; j < num3; j++) {
		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, consumer, NULL);
	}

	sleep(num1);

	return 0;
}
