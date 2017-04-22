//Assignment 1
//concurrency program 1 

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include "mt19937ar.h"
//header files
#define buffer_capacity 32
//set up the capacity of buffer

struct prodcons{
	int buffer[buffer_capacity];
	int size;
	pthread_mutex_t lock;
	pthread_cond_t notempty;
	pthread_cond_t notfull;
};
//set up a struct to initial

int random_number(void){
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;

	char vendor[13];
	eax = 0x01;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	
	if(ecx & 0x40000000)
		return 1;	// assembly language
	else
		return 0;	// mt 19937
}

unsigned long asm_random(void){
	unsigned long number;
	asm("rdrand %0" : "=r"(number));
	return number;
}


void pthread_init(struct prodcons *p){
	pthread_mutex_init(&p->lock, NULL);
	pthread_cond_init(&p->notempty, NULL);
	pthread_cond_init(&p->notfull, NULL);
	p->size = 0;
}

void print_buffer(struct prodcons *p){
	int s;
	printf("buffer: ");
	for(s=0; s<buffer_capacity; s++){
		if(s >= p->size)
			printf("* ");
		else
			printf("%d ", p->buffer[s]);
	}
    	printf("\n");
}

void put(struct prodcons *p, int data, int sleep){				
	pthread_mutex_lock(&p->lock);
	
	if(p->size == buffer_capacity){
		pthread_cond_wait(&p->notfull, &p->lock);
		pthread_cond_signal(&p->notempty);
	}	
	
	p->buffer[p->size] = data;
	p->size++;

	printf("\nproduce an item %d, sleep time is %d\n", data, sleep);
	pthread_cond_signal(&p->notempty);
	
	pthread_mutex_unlock(&p->lock);
}

void get(struct prodcons *p, int sleep){
	int i;
	int data;
    
	pthread_mutex_lock(&p->lock);
    
   	if(p->size==0){
		pthread_cond_wait(&p->notempty, &p->lock);
		pthread_cond_signal(&p->notfull);
	}

	data = p->buffer[0];
	for(i=0; i<p->size; i++)
		p->buffer[i] = p->buffer[i+1];
    	p->size--;

	printf("\ncosnume an item %d, sleep time is %d\n", data, sleep);
	pthread_cond_signal(&p->notfull);

	pthread_mutex_unlock(&p->lock);
}

struct prodcons buffer;

void *producer(void *data){
	int n;
	int d;
	while(1){
		
		if(random_number()==1){
			n = asm_random()%5+3;
			d = asm_random()%20+20;	
		}
		else{
			n = genrand_int32()%5+3;
			d = genrand_int32()%20+20;
		}
		sleep(n);
        put(&buffer,d, n);
		print_buffer(&buffer);
	}
    	return NULL;
}

void *consumer(void *data){
	int n;
    
	while(1){
		if(random_number()==1)
			n = asm_random()%8+2;
		else
			n = genrand_int32()%8+2;
		sleep(n);
    	get(&buffer, n);
		print_buffer(&buffer);
	}
    	return NULL;
}

int main(int argc, char** argv){
   	if (argc != 2) {
	   printf("Need to enter number of threads.\n");
	   return 1;
	}
	int num = atoi(argv[1]);
	if (num < 2) {
	   printf("Can't spawn less than 2 threads.\n");
	   return 1;
	}
	pthread_t* threads;
	threads = (pthread_t*)malloc(num*sizeof(pthread_t));
	pthread_init(&buffer);
	for(int i=0;i<num;i++) {
	   if(i % 2 == 0) {
	      pthread_create(&threads[i], NULL, producer, 0);
	   }
	   else {
	      pthread_create(&threads[i], NULL, consumer, 0);
	   }
	}
	for(int i=0;i<num;i++) {
	   pthread_join(threads[i], NULL);
	}
   	
	return 0;
}
