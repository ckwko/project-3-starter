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
	printf("Num threads: %d\n", n_threads);
	pthread_t threads[n_threads];
	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, 3);
	void *status = NULL;	

	// set thread arguments to pass in later
	struct thread_args *args[n_threads];

	for (int i = 0; i < n_threads; i++) {
		args[i] = (struct thread_args*)malloc(sizeof(struct thread_args));
		args[i]->id = i;
		args[i]->input_chars = input_chars;
		args[i]->input_chars_size = input_chars_size;
		args[i]->zipped_chars = zipped_chars;
		args[i]->zipped_chars_count = zipped_chars_count;
		args[i]->char_frequency = char_frequency;
		args[i]->sec_size = input_chars_size/n_threads;
		args[i]->start = args[i]->id * args[i]->sec_size;
	}

	for (int i = 0; i < n_threads; i++) 
	{
		if (pthread_create(&threads[i], NULL, count_chars, args[i]) != 0) {
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

		// where barrier 
		// how mutex
		struct thread_args *result = ((struct thread_args *)status);
		printf("Elements found: %d\n", result->elements_found);
		for (int i = 0; i < result->elements_found; i++) {
			printf("%c", result->zipped_chars[i].character);
			printf("%d\n", result->zipped_chars[i].occurence);
		}

		printf("thread '%d' exited with '%s'\n", i, (char*)status);
	}
}

void *count_chars(void *arg) 
{
	struct thread_args *args = ((struct thread_args *)arg);
	printf("%s\n", args->input_chars);
	// printf("%d\n", args->start);

	int size = 1;
	args->zipped_chars = (struct zipped_char*)malloc(size * sizeof(struct zipped_char));
	args->zipped_chars[0].character = args->input_chars[args->start];

	int char_pos = 0;

	// add for each occurence, if new letter found add to list
	for (int i = args->start; i < args->sec_size + args->start; i++) {
		if (args->input_chars[i] == args->zipped_chars[char_pos].character) {
			args->zipped_chars[char_pos].occurence++;
		} else {
			size++;
			args->zipped_chars = realloc(args->zipped_chars, size * sizeof(int)); 
			char_pos++;
			args->zipped_chars[char_pos].character = args->input_chars[i];
			args->zipped_chars[char_pos].occurence++;
		}
	}

	args->elements_found = size;

	pthread_exit(args);
}