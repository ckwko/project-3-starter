#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */
void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	printf("%d\n", n_threads);
	pthread_t threads[n_threads];
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, 3);
	void *status = NULL;	

	// set thread arguments to pass in later
	struct thread_args args;
	args.input_chars = input_chars;
	args.input_chars_size = input_chars_size;
	args.zipped_chars = zipped_chars;
	args.zipped_chars_count = zipped_chars_count;
	args.char_frequency = char_frequency;
	args.sec_size = input_chars_size/n_threads;


	for (int i = 0; i < n_threads; i++) 
	{
		args.id = i;
		if (pthread_create(&threads[i], NULL, count_chars, &args) != 0) {
			perror("pthread_create() error");
			exit(1);
		}
	}
	
	for (int i = 0; i < n_threads; i++) 
	{
		if (pthread_join(threads[i], &status) != 0) 
		{
			perror("pthread_create() error");
			exit(3);
		}

		printf("thread '%d' exited with '%s'\n", i, (char*)status);
	}
}

void *count_chars(void *arg) 
{
	struct thread_args* args = ((struct thread_args*)arg);
	printf("%s\n", args->input_chars);
	int start = args->id * args->sec_size;

	for (int i = start; i < args->sec_size + start; i++) {
		printf("%d\n", i);
	}

	char *status = NULL;
	printf("thread() '%d' entered\n", args->id);
	// status = "testing";
	pthread_exit(status);
}